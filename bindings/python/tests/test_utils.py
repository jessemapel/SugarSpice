from pyspiceql import toUpper, toLower

def test_toUpper():
    assert isinstance(toUpper('Some Test String'), str)

def test_toLower():
    assert isinstance(toLower('Some Test String'), str)