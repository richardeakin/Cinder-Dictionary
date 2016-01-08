#### Cinder-Dictionary

Work-in-progress class to support string-to-any maps with a convenient syntax.

#####Usage

You can convert json into a `Dictionary`. For example the following:

```json
{
  "a" : 2,
  "b" : 3.14,
  "c" : "kite",
  "nested" : {
    "blah" : "When will it snow?",
    "days" : 204
  },
  "mixedArray" : [1.5, 0, "blue"],
  "oddNumbers" : [1, 3, 5, 7]
}
```

Can be loaded and queried like:

```cpp

auto dict = mason::Dictionary::convert<Json::Value>( loadFile( filePath ) );

// get values by type
int a = dict.get<int>( "a" );
float b = dict.get<float>( "b" );
string c = dict.get<string>( "c" );

// nested Dictionary, copied
auto nested = dict.get<ma::Dictionary>( "nested" );

// nested Dictionary without copying
const auto &nestedRef = dict.getStrict<ma::Dictionary>( "nested" );

// vector with mixed types
auto mixedArray = dict.get<vector<boost::any>>( "mixedArray" );

// vector with uniform elemnt types
auto oddNumbers = dict.get<vector<int>>( "oddNumbers" );

```
