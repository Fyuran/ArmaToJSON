/** @mainpage ArmaToJSON Library
 * @brief A library for converting JSON data to Arma scripting format and handling packets.
 *
 * This library provides utilities for packet management, file operations, logging, and output
 * handling for interfacing with Arma game extensions.
 */

#pragma once
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <nlohmann/json.hpp>
#include <string>

/// @namespace ArmaToJSON
/// @brief ArmaToJSON library namespace.
namespace ArmaToJSON {

class Packet;
template <typename T>
concept NotPacket = !std::same_as<std::remove_cvref_t<T>, Packet>;

template <typename T>
concept JsonCompatible = requires(T t) {
	{ nlohmann::json(t) } -> std::same_as<nlohmann::json>;
};

template <typename T>
concept JSONOnly = NotPacket<T> && JsonCompatible<T>;

/// @class Packet
/// @brief Represents a data packet with header, body, and metadata.
class Packet {
  public:
	/// @brief Default constructor.
	Packet();

	/// @brief Move Copy constructor.
	/// @param packet The other packet.
	Packet(Packet &&packet);

	/// @brief Copy constructor by reference.
	/// @param packet The other packet.
	Packet(const Packet &packet);

	/// @brief Constructs a Packet from a string view.
	/// @param data The data string.
	Packet(const std::string_view data);

	/// @brief Constructs a Packet from a rvalue JSON object.
	/// @param json The JSON data.
	explicit Packet(nlohmann::json &&json);

	/// @brief Constructs a Packet from a JSON object.
	/// @param json The JSON data.
	explicit Packet(const nlohmann::json &json);

	/// @brief Returns a reference to the object element with specified key.
	/// @param key The property key.
	nlohmann::json &operator[](const std::string_view key);

	/// @brief Returns a const reference to the object element with specified key.
	/// @param key The property key.
	const nlohmann::json &operator[](const std::string_view key) const;

	/// @brief Gets the packet size by the string size of _data dump.
	/// @return The packet size.
	size_t size() const;

	/// @brief Gets _data dump.
	/// @return _data dumped.
	std::string dump() const;

	/// @brief Flattens _data then returns a copy of it.
	/// @return _data flattened.
	nlohmann::json flatten();

	/// @brief Unflattens _data then returns a copy of it.
	/// @return _data unflattened.
	nlohmann::json unflatten();

	/// @brief Gets a body property.
	/// @tparam The type that the return will be cast to.
	/// @param key The property key.
	/// @param def The default value is key is not found
	/// @return The property value cast to T.
	template <JSONOnly T = nlohmann::json>
	T value(const std::string_view key, nlohmann::json def) const {
		return _data.value(key, def).get<T>();
	};

	/// @brief Retrieves all keys at first depth of body
	/// @return The keys of body
	std::vector<std::string> keys() const;

	/// @brief Checks if a body property exists.
	/// @param key The property key.
	/// @return True if the property exists.
	bool hasKey(const std::string_view key) const;

	/// @brief Gets _data const reference.
	/// @return _data const reference.
	const nlohmann::json &getData() const;

	/// @brief Sets _data to a new json.
	/// @param data The new json to be copied from.
	void setData(const nlohmann::json& data);

	/// @brief Sets _data to a new json by rvalue ref.
	/// @param data The new json to be moved.
	void setData(const nlohmann::json&& data);

	/// @brief Inserts all values from JSON object j
	/// @param j JSON object to read values from.
	/// @param merge_objects Optional boolean when false, existing keys are overwritten.
	/// When merge_objects is true(default), recursively merges objects with common keys.
	void merge(const nlohmann::json &j, bool merge_objects = true);

	/// @brief Inserts all values from other Packet's _data into this.
	/// @param other Packet object to read values from.
	/// @param merge_objects Optional boolean when false, existing keys are overwritten.
	/// When merge_objects is true(default), recursively merges objects with common keys.
	void merge(const Packet &other, bool merge_objects = true);

	/// @brief Clears the contents of data
	void clear();

	/// @brief Checks if the dump of the _data is empty
	/// @return true if empty dump string.
	bool empty();

	/// @brief Post-Increments ID then returns the string ID.
	/// @return The ID.
	static const std::string getNewID();

	/// @brief Returns Packet into a string.
	operator std::string() const;

	Packet &operator=(const Packet &other);
	Packet &operator=(Packet &&other) noexcept;
	Packet &operator+=(const Packet &other);
	Packet &operator+=(const nlohmann::json &other);
  private:
	/// @brief Packet JSON object.
	nlohmann::json _data;

	/// @brief used for the packet storage, increases everytime getNewID is called
	static uint32_t ID;
};
std::ostream &operator<<(std::ostream &os, const Packet &p);
Packet operator+(Packet lhs, const Packet &rhs);
Packet operator+(Packet lhs, const nlohmann::json &rhs);
} // namespace ArmaToJSON

template <> struct fmt::formatter<ArmaToJSON::Packet> : fmt::ostream_formatter {};

template <>
struct fmt::formatter<nlohmann::json> {
    int indent = -1;  // -1 = compact, ≥0 = pretty-print

    constexpr auto parse(fmt::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            indent = 0;
            while (it != ctx.end() && *it >= '0' && *it <= '9') {
                indent = indent * 10 + (*it++ - '0');
            }
        }
        return it;
    }

    auto format(const nlohmann::json& j, fmt::format_context& ctx) const {
        return fmt::format_to(ctx.out(), "{}", j.dump(indent));
    }
};