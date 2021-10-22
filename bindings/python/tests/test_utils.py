from pyspiceql import toUpper, toLower

def test_toUpper():
    assert type(toUpper('Some Test String')) is str

def test_toLower():
    assert type(toLower('Some Test String')) is str