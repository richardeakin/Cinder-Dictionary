/*
 Copyright (c) 2014, Richard Eakin - All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided
 that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "cinder/Filesystem.h"
#include "cinder/DataSource.h"
#include "cinder/Exception.h"
#include "cinder/System.h"

#include <memory>
#include <map>

#include <boost/any.hpp>

namespace mason {

typedef std::shared_ptr<class Dictionary> DictionaryRef;

//! Object for storing dynamic data in a hierarchical form, key = string, value = any.
class Dictionary {
  public:
	//! Constructs an empty Dictionary on the stack
	Dictionary();
	//! Creates an empty Dictionary and returns it in a shared_ptr
	static DictionaryRef	create()	{ return std::make_shared<Dictionary>(); }

	template<typename DataT>
	static Dictionary		convert( const DataT &data );

	template<typename DataT>
	static Dictionary		convert( const ci::DataSourceRef &dataSource );

	template<typename DataT>
	static Dictionary		convert( const ci::fs::path &filePath );

	// TODO: possible to reuse above converts but store in shared_ptr?
//	template<typename DataT, typename... Args>
//	static DictionaryRef	convertShared( Args... )

	template<typename T>
	void		set( const std::string &key, const T &value );

	//! Returns a copy of the value associated with T. Some type conversions are allowed (like double -> float), hency why a copy is necessary.
	template<typename T>
	T	get( const std::string &key ) const;
	//! Returns a reference to the value associated with T. The typeid must match exactly.
	template<typename T>
	const T&	getStrict( const std::string &key ) const;

	const std::type_info&   getType( const std::string &key ) const;

	bool contains( const std::string &key ) const;

	// TODO: need to sort out what is happening with return versus const-ref before these are useful
	// - could use getStrict() but more likely to throw
//	template<typename T>
//	T& operator[]( const std::string &key )			    { return get<T>( key ); }
//	template<typename T>
//	const T& operator[]( const std::string &key ) const { return get<T>( key ); }

	size_t getSize() const  { return mData.size(); }
	std::vector<std::string>	getAllKeys() const;

	bool isEmpty() const		{ return mData.empty(); }

	// TODO: implement
	//std::string	toString() const;

  private:

	std::map<std::string, boost::any>	mData;
};

class DictionaryExc : public ci::Exception {
  public:
	DictionaryExc( const std::string &description )
			: Exception( description )
	{ }
};

class DictionaryBadTypeExc : public DictionaryExc {
  public:
	DictionaryBadTypeExc( const std::string &key, const boost::any &value, const std::type_info &typeInfo )
		: DictionaryExc( "" )
	{
		std::string descr = "Failed to convert value for key '" + key + "', from native type '";
		descr += ci::System::demangleTypeName( value.type().name() ) + "' to requested type '";
		descr += ci::System::demangleTypeName( typeInfo.name() ) + "'.";
		setDescription( descr );
	}
};

// ----------------------------------------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------------------------------------

namespace detail {

template<typename T>
bool getValue( const boost::any &value, T *result )
{
	if( value.type() != typeid( T ) ) {
		return false;
	}

	*result = boost::any_cast<T>( value );
	return true;
}

template<typename T>
bool getValue( const boost::any &value, std::vector<T> *result )
{
	if( value.type() != typeid( std::vector<boost::any> ) ) {
		return false;
	}

	auto anyVec = boost::any_cast<std::vector<boost::any>>( value );
	result->resize( anyVec.size() );
	for( int i = 0; i < anyVec.size(); i++ ) {
		getValue( anyVec[i], &( (*result)[i] ) );
	}

	return true;
}

bool getValue( const boost::any &value, float *result );
bool getValue( const boost::any &value, double *result );
bool getValue( const boost::any &value, std::vector<boost::any> *result );

} // namespace mason::detail

template<typename T>
void Dictionary::set( const std::string &key, const T &value )
{
	mData[key] = value;
}

template<typename T>
T Dictionary::get( const std::string &key ) const
{
	auto it = mData.find( key );
	if( it == mData.end() ) {
		throw DictionaryExc( "no key named '" + key + "'" );
	}

	T result;
	const auto &value = it->second;

	if( ! detail::getValue( value, &result ) ) {
		throw DictionaryBadTypeExc( key, value, typeid( T ) );
	}

	return result;
}

template<typename T>
const T& Dictionary::getStrict( const std::string &key ) const
{
	auto it = mData.find( key );
	if( it == mData.end() ) {
		throw DictionaryExc( "no key named '" + key + "'" );
	}

	const auto &value = it->second;

	if( value.type() != typeid( T ) ) {
		throw DictionaryBadTypeExc( key, value, typeid( T ) );
	}

	return boost::any_cast<const T&>( value );
}


} // namespace mason
