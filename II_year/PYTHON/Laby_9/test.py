import pytest 

def addition(a: int | float, b: int | float) -> int | float:  
    return a + b

@pytest.mark.parametrize( "a, b, expected",  
    [  
        (1, 2, 3),  
        (5, -1, 4),  
    ],)  
def test_addition(a, b, expected):  
    assert addition(a, b) == expected  