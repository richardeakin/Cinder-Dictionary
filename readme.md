#### Cinder-Dictionary

Work-in-progress class to support string-to-any maps with a convenient syntax.

#####Usage

```cpp

auto dict = mason::Dictionary::convert<Json::Value>( loadFile( filePath ) );

CI_LOG_I( "number of elements: " << dict.getSize() );

int a = dict.get<int>( "a" );
CI_LOG_I( "a: " << a );
float b = dict.get<float>( "b" );
CI_LOG_I( "b: " << b );
string c = dict.get<string>( "c" );
CI_LOG_I( "c: " << c );

// nested Dictionary, copied
auto nested = dict.get<ma::Dictionary>( "nested" );
CI_LOG_I( "nested blah string: " << nested.get<string>( "blah" ) );

// nested Dictionary without copying
const auto &nestedRef = dict.getStrict<ma::Dictionary>( "nested" );
CI_LOG_I( "nestedRef days: " << nestedRef.get<int>( "days" ) );

// vector with mixed types
auto mixedArray = dict.get<vector<boost::any>>( "mixedArray" );

// vector with uniform elemnt types
auto oddNumbers = dict.get<vector<int>>( "oddNumbers" );

```