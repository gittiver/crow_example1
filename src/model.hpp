#include <optional>
#include <string>
#include <map>
#include <mutex>
#include <cstdint>
#include <tl/expected.hpp>
#include "crow.h"


enum class Err {
  NOT_FOUND, ALREADY_EXISTS, JSON_PARSE_ERROR, JSON_MISSING_KEY
};

struct Post {
  uint64_t id;
  std::optional<std::string> title;
  std::optional<std::string> content;

  [[nodiscard]] crow::json::wvalue to_json() const;
  static tl::expected<Post, Err> from_json(const std::string &string);
};

typedef std::map<uint64_t, Post> Posts;

class Model {
  Posts posts_;
  uint64_t next_id{1};
  std::mutex mtx;

public:

  [[nodiscard]] const Posts &posts() const {
    return posts_;
  }

  tl::expected<Post, Err> read(uint64_t id);

  tl::expected<uint64_t, Err> create(const Post &post);

  tl::expected<uint64_t, Err> update(const Post &post);

  bool delete_(uint64_t id);

  std::string get_token() {
    return "token tbd";
  }
};
  
