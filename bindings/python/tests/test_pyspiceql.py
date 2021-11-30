from pyspiceql import getMissionConfig, getKernelList

def test_jsonConversion():
    lro_config = getMissionConfig('lro')
    assert isinstance(lro_config, dict)
    kernel_list = getKernelList(lro_config)
    assert isinstance(kernel_list, tuple)