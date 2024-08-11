#include "model.hpp"



tl::expected <uint64_t, Err> Model::create(const Post &post) {
  const std::lock_guard<std::mutex> g(mtx);
  Post p = post;
  p.id = next_id++;
  posts_[p.id] = p;
  return p.id;
}

tl::expected <Post, Err> Model::read(uint64_t id) {
  const std::lock_guard<std::mutex> g(mtx);
  auto it = posts_.find(id);
  if (it == posts_.end())
    return tl::unexpected(Err::NOT_FOUND);
  else
    return it->second;
}

tl::expected <uint64_t, Err> Model::update(const Post &post) {
  const std::lock_guard<std::mutex> g(mtx);

  auto it = posts_.find(post.id);
  if (it == posts_.end())
    return tl::unexpected(Err::NOT_FOUND);
  else {
    it->second = post;
    return post.id;
  }
}

bool Model::delete_(uint64_t id) {
  const std::lock_guard<std::mutex> g(mtx);
  auto it = posts_.find(id);
  if (it == posts_.end()) {
    return false;
  } else {
    posts_.erase(it);
    return true;
  }
}

crow::json::wvalue Post::to_json() const {
  crow::json::wvalue o;
  o["id"] = id;
  if (title)
    o["title"] = *title;
  if (content)
    o["content"] = *content;
  return o;
}

tl::expected <Post, Err> Post::from_json(const std::string &string) {
  Post post;
  auto json = crow::json::load(string);
  if (!json || !json.has("id")) {
    return tl::unexpected(Err::JSON_PARSE_ERROR);
  } else {
    try {
      post.id = json["id"].u();
      post.title = json["title"].s();
      post.content = json["content"].s();

      return post;
    } catch (std::exception &e) {
      return tl::unexpected(Err::JSON_PARSE_ERROR);
    }
  }
}

crow::json::wvalue Posts::to_json() const {
  crow::json::wvalue::list v;
  for (const auto &post: *this) {
    v.push_back(post.second.to_json());
  }
  crow::json::wvalue r(v);
  return r;
}


std::string Model::get_token() {
  return "token tbd";
}