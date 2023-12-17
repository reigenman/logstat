import datetime as dt
import pathlib
import random
import json
import dataclasses


@dataclasses.dataclass
class GenerateParams:
    start_date: dt.date
    days: int
    fact_per_day: int
    facts_number: int
    props_per_fact: int


def generate_log(out_path: pathlib.Path, log_params: GenerateParams) -> None:
    with out_path.open("w", encoding="utf-8") as file:
        for day in range(log_params.days):
            for fact_i in range(log_params.fact_per_day):
                sec = random.randint(0, 86400 - 1)
                ts_dt = dt.datetime.combine(log_params.start_date, dt.time(), tzinfo=dt.timezone.utc) + dt.timedelta(
                    days=day, seconds=sec)
                record = {"ts_fact": int(ts_dt.timestamp())}
                for prop_i in range(log_params.props_per_fact):
                    record['props'] = {f'prop{i}': prop_i + i for i in range(1, 11)}
                    for fact_item in range(log_params.facts_number):
                        record['fact_name'] = f'fact_{fact_item}'
                        file.write(f'{json.dumps(record)}\n')


def big_file():
    for i in range(3):
        generate_log(pathlib.Path(f'../bin/log{i}.log'),
                     GenerateParams(start_date=dt.date(year=1985 + i, month=7, day=24),
                                    days=5,
                                    fact_per_day=1000,
                                    facts_number=10, props_per_fact=100))


def small_file():
    generate_log(pathlib.Path('../bin/log2.log'), GenerateParams(start_date=dt.date(year=1985, month=8, day=24),
                                                                 days=1,
                                                                 fact_per_day=10,
                                                                 facts_number=10, props_per_fact=1))


if __name__ == '__main__':
    big_file()
