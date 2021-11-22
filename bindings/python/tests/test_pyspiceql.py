from pyspiceql import toUpper, toLower, getMissionConfig, getKernelList

def test_toUpper():
    assert isinstance(toUpper('Some Test String'), str)

def test_toLower():
    assert isinstance(toLower('Some Test String'), str)

def test_jsonConversion():
    lro_config = getMissionConfig('lro')
    assert isinstance(lro_config, dict)
    kernel_list = getKernelList(lro_config)
    assert isinstance(kernel_list, tuple)