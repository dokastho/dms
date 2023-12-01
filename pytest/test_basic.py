import os


def run_command(cmd: str):
    x = os.system(cmd)
    return (x & 0xFF00) >> 8


def test_basic():
    cmd = "./bin/test_basic"
    assert run_command(cmd) == 0
    pass
