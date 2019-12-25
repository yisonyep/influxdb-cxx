///
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDBFactory.h"
#include <functional>
#include <string>
#include <memory>
#include "UriParser.h"
#include "HTTP.h"
#include <map>

#ifdef INFLUXDB_WITH_BOOST
#include "UDP.h"
#include "UnixSocket.h"
#endif

namespace influxdb 
{

#ifdef INFLUXDB_WITH_BOOST
	std::unique_ptr<Transport> withUdpTransport(const http::url& uri) {
		//return std::make_unique<transports::UDP>(uri.host, uri.port);
		return std::unique_ptr<transports::UDP>(new transports::UDP(uri.host, uri.port));
	}

	std::unique_ptr<Transport> withUnixSocketTransport(const http::url& uri) {
		//return std::make_unique<transports::UnixSocket>(uri.path);
		return std::unique_ptr<transports::UnixSocket>(new transports::UnixSocket(uri.path));
	}
#else
std::unique_ptr<Transport> withUdpTransport(const http::url& /*uri*/) {
  throw std::runtime_error("UDP transport requires Boost");
}

std::unique_ptr<Transport> withUnixSocketTransport(const http::url& /*uri*/) {
  throw std::runtime_error("Unix socket transport requires Boost");
}
#endif

	std::unique_ptr<Transport> withHttpTransport(const http::url& uri) {
		//auto transport = std::make_unique<transports::HTTP>(uri.url);
		auto transport = std::unique_ptr<transports::HTTP>(new transports::HTTP(uri.url));

		if (!uri.user.empty()) {
			transport->enableBasicAuth(uri.user + ":" + uri.password);
		}

  if (uri.protocol == "https") {
    transport->enableSsl();
  }
  return transport;
}

std::unique_ptr<Transport> InfluxDBFactory::GetTransport(std::string url) {
  static const std::map<std::string, std::function<std::unique_ptr<Transport>(const http::url&)>> map = {
    {"udp", withUdpTransport},
    {"http", withHttpTransport},
    {"https", withHttpTransport},
    {"unix", withUnixSocketTransport},
  };

  http::url parsedUrl = http::ParseHttpUrl(url);
  if (parsedUrl.protocol.empty()) {
    throw std::runtime_error("Ill-formed URI");
  }   

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator == map.end()) {
    throw std::runtime_error("Unrecognized backend " + parsedUrl.protocol);
  }

  return iterator->second(parsedUrl);
}

	std::unique_ptr<InfluxDB> InfluxDBFactory::Get(std::string url)
	{
		//return std::make_unique<InfluxDB>(InfluxDBFactory::GetTransport(url));
		return std::unique_ptr<InfluxDB>(new InfluxDB(InfluxDBFactory::GetTransport(url)));
	}

} // namespace influxdb
