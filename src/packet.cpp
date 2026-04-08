#include <cstddef>
#include <nlohmann/json_fwd.hpp>
#include <packet.hpp>

using JSON = nlohmann::json;
namespace ArmaToJSON {

// Merely initialization
uint32_t Packet::ID = 0;

Packet::Packet() : _data(JSON::value_t::object) {}

Packet::Packet(Packet &&packet) : _data(std::move(packet._data)) {}

Packet::Packet(const Packet &packet) : _data(packet._data) {}

Packet::Packet(JSON &&json) : _data(std::move(json)) {}

Packet::Packet(const JSON &json) : _data(json) {}

Packet::Packet(const std::string_view data) : Packet{JSON::parse(data.empty() ? "{}" : data)} {}

bool Packet::hasKey(const std::string_view key) const { return _data.contains(key); }

JSON &Packet::operator[](const std::string_view key) { return _data[key]; }

const JSON &Packet::operator[](const std::string_view key) const { return _data[key]; }

std::vector<std::string> Packet::keys() const {
	std::vector<std::string> keys{};

	for (const auto &[key, val] : _data.items()) {
		keys.push_back(key);
	}
	fmt::print("{}", ArmaToJSON::Packet{});

	return keys;
}

size_t Packet::size() const { return dump().size(); }

std::string Packet::dump() const {
	return _data.dump(-1, ' ', true, nlohmann::json::error_handler_t::ignore);
}

nlohmann::json Packet::flatten() {
	_data = _data.flatten();
	return _data;
}
nlohmann::json Packet::unflatten() {
	_data = _data.unflatten();
	return _data;
}

const JSON &Packet::getData() const { return _data; }

void Packet::setData(const nlohmann::json &data) { _data = JSON(data); }

void Packet::setData(const nlohmann::json &&data) { _data = JSON(std::move(data)); }

void Packet::merge(const nlohmann::json &j, bool merge_objects) { _data.update(j, merge_objects); }
void Packet::merge(const Packet &other, bool merge_objects) {
	_data.update(other._data, merge_objects);
}

void Packet::clear() {
	_data.clear();
	_data = nlohmann::json::value_t::object;
}

bool Packet::empty() { return dump().empty(); }

const std::string Packet::getNewID() {
	auto now = std::chrono::system_clock::now();
	return fmt::format("{}-{:%T}", Packet::ID++, now);
}

Packet::operator std::string() const { return dump(); }

Packet &Packet::operator=(const Packet &other) {
	if (this == &other)
		return *this;
	_data = other._data;

	return *this;
}

Packet &Packet::operator=(Packet &&other) noexcept {
	if (this == &other)
		return *this;
	_data = std::move(other._data);

	return *this;
}

std::ostream &operator<<(std::ostream &os, const Packet &p) { return os << p.dump(); }

Packet &Packet::operator+=(const Packet &other) {
	merge(other._data);
	return *this;
}

Packet operator+(Packet lhs, const Packet &rhs) {
	lhs += rhs;
	return lhs;
}

Packet &Packet::operator+=(const JSON &other) {
	merge(other);
	return *this;
}

Packet operator+(Packet lhs, const JSON &rhs) {
	lhs += rhs;
	return lhs;
}
} // namespace ArmaToJSON
