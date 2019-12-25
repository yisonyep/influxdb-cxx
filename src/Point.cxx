///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Point.h"

#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>

using namespace std;
namespace influxdb
{

	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	//template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

Point::Point(const std::string& measurement) :
  mMeasurement(measurement), mTimestamp(Point::getCurrentTimestamp())
{
  //mValue = {};
  mTags = {};
  mFields = {};
}

	Point&& Point::addField(std::string name, int  value)
	{
		std::stringstream convert;
		if (!mFields.empty()) convert << ",";

		convert << name << "=";
		convert << value << 'i';
		mFields += convert.str();
		return std::move(*this);

	}
	Point&& Point::addField(std::string name, string  value)
	{
		std::stringstream convert;
		if (!mFields.empty()) convert << ",";

		convert << name << "=";
		convert << '"' << value << '"';
		mFields += convert.str();
		return std::move(*this);
	}

	Point&& Point::addField(std::string name, double value)
	{
		std::stringstream convert;
		if (!mFields.empty()) convert << ",";

		convert << name << "=";
		convert << value;
		mFields += convert.str();
		return std::move(*this);
	}

	//Point&& Point::addField(std::string name, std::variant<int, std::string, double> value)
	//{
	//	std::stringstream convert;
	//	if (!mFields.empty()) convert << ",";

	//	convert << name << "=";
	//	std::visit(overloaded{
	//	  [&convert](int value) { convert << value << 'i'; },
	//	  [&convert](double value) { convert << value; },
	//	  [&convert](const std::string& value) { convert << '"' << value << '"'; },
	//		}, value);
	//	mFields += convert.str();
	//	return std::move(*this);
	//}

	Point&& Point::addTag(std::string key, std::string value)
	{
		mTags += ",";
		mTags += key;
		mTags += "=";
		mTags += value;
		return std::move(*this);
	}

Point&& Point::setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp)
{
  mTimestamp = timestamp;
  return std::move(*this);
}

auto Point::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now())
{
  return std::chrono::system_clock::now();
}

std::string Point::toLineProtocol() const
{
  return mMeasurement + mTags + " " + mFields + " " + std::to_string(
    std::chrono::duration_cast <std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count()
  );
}

std::string Point::getName() const
{
  return mMeasurement;
}

std::chrono::time_point<std::chrono::system_clock> Point::getTimestamp() const
{
  return mTimestamp;
}

std::string Point::getFields() const
{
  return mFields;
}

} // namespace influxdb
