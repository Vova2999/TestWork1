#pragma once

#include <map>
#include <algorithm>

using namespace std;

template <typename TValue>
class ComplexMap
{
	class ValueType
	{
	public:
		virtual ~ValueType()
		{ }
	};

	class SingleValueType : public ValueType
	{
	public:
		TValue Value;
		
		SingleValueType(TValue value)
			: Value{value}
		{ }

		virtual ~SingleValueType()
		{ }
	};

	class ArrayValueType : public ValueType
	{
	public:
		TValue* Values;
		int Size;
		
		ArrayValueType(TValue* values, int size)
			: Size{ size }
		{
			Values = new TValue[size];
			memcpy_s(Values, size * sizeof(TValue), values, size * sizeof(TValue));
		}
		
		virtual ~ArrayValueType()
		{
			delete[] Values;
		}
	};

	class StringValueType : public ValueType
	{
	public:
		char* Line;

		StringValueType(char* line)
		{
			int length = strlen(line) + 1;
			Line = new char[length];
			strcpy_s(Line, length, line);
		}

		virtual ~StringValueType()
		{
			delete[] Line;
		}
	};

	map<int, ValueType*> valuesMap;

	void AddValueType(int key, ValueType* valueType)
	{
		pair<map<int, ValueType*>::iterator, bool> inserted = valuesMap.insert(pair<int, ValueType*>(key, valueType));
		if (inserted.second == false) {
			delete valueType;
			throw "Key already exists";
		}
	}

	void AddValueTypeOrReplace(int key, ValueType* valueType)
	{
		pair<map<int, ValueType*>::iterator, bool> inserted = valuesMap.insert(pair<int, ValueType*>(key, valueType));
		if (inserted.second == false) {
			delete inserted.first->second;
			inserted.first->second = valueType;
		}
	}

	template<typename TValueType>
	TValueType* GetValueType(int key)
	{
		map<int, ValueType*>::iterator value = valuesMap.find(key);
		if (value == valuesMap.end())
			throw "Key not found";

		TValueType* valueType = dynamic_cast<TValueType*>(value->second);
		if (valueType == nullptr)
			throw "Invalid value type";

		return valueType;
	}

	template<typename TValueType>
	TValueType* GetValueTypeOrNullptr(int key)
	{
		map<int, ValueType*>::iterator value = valuesMap.find(key);
		if (value == valuesMap.end())
			return nullptr;

		TValueType* valueType = dynamic_cast<TValueType*>(value->second);
		if (valueType == nullptr)
			return nullptr;

		return valueType;
	}

public:
	~ComplexMap()
	{
		RemoveAll();
	}

	int GetSize()
	{
		return valuesMap.size();
	}

	void AddValue(int key, TValue value)
	{
		AddValueType(key, new SingleValueType(value));
	}
	void AddArray(int key, TValue* values, int size)
	{
		AddValueType(key, new ArrayValueType(values, size));
	}
	void AddString(int key, char* line)
	{
		AddValueType(key, new StringValueType(line));
	}
	
	void AddValueOrReplace(int key, TValue value)
	{
		AddValueTypeOrReplace(key, new SingleValueType(value));
	}
	void AddArrayOrReplace(int key, TValue* values, int size)
	{
		AddValueTypeOrReplace(key, new ArrayValueType(values, size));
	}
	void AddStringOrReplace(int key, char* line)
	{
		AddValueTypeOrReplace(key, new StringValueType(line));
	}
	
	TValue GetValue(int key)
	{
		SingleValueType* singleValue = GetValueType<SingleValueType>(key);
		return singleValue->Value;
	}
	TValue* GetArray(int key, int* size)
	{
		ArrayValueType* arrayValue = GetValueType<ArrayValueType>(key);
		*size = arrayValue->Size;
		return arrayValue->Values;
	}
	char* GetString(int key)
	{
		StringValueType* stringValue = GetValueType<StringValueType>(key);
		return stringValue->Line;
	}

	bool TryGetValue(int key, TValue* value)
	{
		SingleValueType* singleValue = GetValueTypeOrNullptr<SingleValueType>(key);
		if (singleValue == nullptr)
			return false;

		*value = singleValue->Value;
		return true;
	}
	bool TryGetArray(int key, TValue** values, int* size)
	{
		ArrayValueType* arrayValue = GetValueTypeOrNullptr<ArrayValueType>(key);
		if (arrayValue == nullptr)
			return false;
		
		*values = arrayValue->Values;
		*size = arrayValue->Size;
		return true;
	}
	bool TryGetString(int key, char** line)
	{
		StringValueType* stringValue = GetValueTypeOrNullptr<StringValueType>(key);
		if (stringValue == nullptr)
			return false;
		
		*line = stringValue->Line;
		return true;
	}

	void Remove(int key)
	{
		map<int, ValueType*>::iterator value = valuesMap.find(key);
		if (value == valuesMap.end())
			throw "Key not found";

		valuesMap.erase(value);
	}
	bool TryRemove(int key)
	{
		map<int, ValueType*>::iterator value = valuesMap.find(key);
		if (value == valuesMap.end())
			return false;

		valuesMap.erase(value);
		return true;
	}
	void RemoveAll()
	{
		for_each(valuesMap.begin(), valuesMap.end(),
			[](pair<int, ValueType*> value) {
				delete value.second;
			});
		valuesMap.clear();
	}
};
