#include "crow/http_response.h"
#include "crow/json.h"
#include <optional>

#define CROW_MAIN

#include "crow.h"

#include "model.hpp"

Model model;

crow::json::wvalue error2json(const std::string &errstring) {
  crow::json::wvalue r;
  r["error"] = errstring;
  return r;
}

// Authentication utilities
bool is_valid(const std::string &username, const std::string &password) {
  return username == "frank" && password == "frank";
}

bool validate_authentication(const crow::request &request, crow::response &response) {
  std::string myauth = request.get_header_value("Authorization");
  if (myauth.empty()) {
    response.code = crow::status::UNAUTHORIZED;
    response.set_header("WWW-Authenticate",
                        "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
    return false;
  } else {
    // Cut off starting "Basic "
    std::string mycreds = myauth.substr(6);
    // decode base64
    std::string d_mycreds = crow::utility::base64decode(mycreds, mycreds.size());
    // Now that we have our username:password string,
    // we only need to separate it into 2 different strings
    // and verify their validity:
    size_t found = d_mycreds.find(':');
    std::string username = d_mycreds.substr(0, found);
    std::string password = d_mycreds.substr(found + 1);
    return is_valid(username, password);
  }
}

#define CHECK_AUTHENTICATION(request) \
{ \
  crow::response response; \
  if (!validate_authentication(req, response)) { \
    return response; \
  } \
}
// End of Authentication utilities

int main() {
  crow::SimpleApp app;
  crow::mustache::set_global_base("template");
  CROW_ROUTE(app, "/posts").methods(crow::HTTPMethod::GET)
      ([]() {
        auto page = crow::mustache::load("posts.html");
        crow::mustache::context ctx;
        ctx["posts"]= model.posts().to_json();

        return page.render(ctx);
      });

  CROW_ROUTE(app, "/post/edit/<int>").methods(crow::HTTPMethod::GET,crow::HTTPMethod::POST)
      ([](uint64_t id) {
        auto page = crow::mustache::load("post_edit.html");
        crow::mustache::context ctx;
        auto post = model.read(id);
        if (!post)
          ctx["error"] = "post not found";
        else
          ctx["post"] = post.value().to_json();

        return page.render(ctx);
      });

  CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::POST)
      ([](const crow::request &req) {
        auto post = Post::from_json(req.body);
        if (!post) {
          return crow::response(crow::BAD_REQUEST, error2json("json parse_error"));
        } else {
          auto result = model.create(*post);
          if (result)
            return crow::response(crow::OK);
          else
            return crow::response(crow::BAD_REQUEST, error2json("json parse_error"));

        }
      });

  CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::PUT)
      ([](const crow::request &req) {
        crow::status rc;
        crow::json::wvalue json_response;
        auto post = Post::from_json(req.body);
        if (!post) {
          rc = crow::BAD_REQUEST;
          json_response = error2json("json parse error");
        } else {
          auto updated = model.update(post.value());
          if (!updated) {
            rc = crow::NOT_FOUND;
            json_response = error2json("post not found");
          } else {
            rc = crow::OK;
            json_response["id"] = updated.value();
          }
        }
        return crow::response(rc, json_response);
      });

  CROW_ROUTE(app, "/api/posts/<int>").methods(crow::HTTPMethod::GET)
      ([](uint64_t id) {
        auto g = model.read(id);
        if (!g)
          return crow::response(crow::BAD_REQUEST, error2json("item not found"));
        else
          return crow::response(crow::OK, g->to_json());
      });

  CROW_ROUTE(app, "/api/posts/<int>").methods(crow::HTTPMethod::DELETE)
      ([](const crow::request & /*req*/, int id) {
        bool success = model.delete_(id);
        if (!success) {
          return crow::response(crow::NOT_FOUND, error2json("item not found"));
        } else {
          return crow::response(crow::OK);
        }
      });

  CROW_ROUTE(app, "/api/login").methods(crow::HTTPMethod::GET,
                                    crow::HTTPMethod::POST)
      ([](const crow::request &req) {
         CHECK_AUTHENTICATION(req);
         // logged in successfully
         crow::json::wvalue w;
         w["token"] = model.get_token();

         return crow::response(crow::status::OK, w);
       }

      );

  CROW_ROUTE(app, "/api/do_authenticated").
      methods(crow::HTTPMethod::POST, crow::HTTPMethod::GET)
      ([](const crow::request &req) {
        CHECK_AUTHENTICATION(req);

        CROW_LOG_INFO << "do authenticated after successful authentication";
        return crow::response(crow::status::OK);
      });

  app.port(18080).run();

}
