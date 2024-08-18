#include "crow/http_response.h"
#include "crow/json.h"
#include <optional>

#define CROW_MAIN

#include "crow.h"
#include "auth.hpp"
#include "model.hpp"

Model model;

crow::json::wvalue error2json(const std::string &errstring) {
  crow::json::wvalue r;
  r["error"] = errstring;
  return r;
}

int main() {
  crow::App<LoginRequiredMiddleware> app;

  CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Get)
      ([]() {
        return model.posts().to_json();
      });

  CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Post)
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

  CROW_ROUTE(app, "/api/posts").methods(crow::HTTPMethod::Put)
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

  CROW_ROUTE(app, "/api/posts/<int>").methods(crow::HTTPMethod::Get)
      ([](uint64_t id) {
        auto g = model.read(id);
        if (!g)
          return crow::response(crow::BAD_REQUEST, error2json("item not found"));
        else
          return crow::response(crow::OK, g->to_json());
      });

  CROW_ROUTE(app, "/api/posts/<int>").methods(crow::HTTPMethod::Delete)
      ([](const crow::request & /*req*/, int id) {
        bool success = model.delete_(id);
        if (!success) {
          return crow::response(crow::NOT_FOUND, error2json("item not found"));
        } else {
          return crow::response(crow::OK);
        }
      });

  CROW_ROUTE(app, "/api/login")
      .methods(crow::HTTPMethod::Get, crow::HTTPMethod::Post)
      .CROW_LOGIN_REQUIRED(app)

          ([]() {
             // logged in successfully
             crow::json::wvalue w;
             w["token"] = model.get_token();

             return crow::response(crow::status::OK, w);
           }

          );

  CROW_ROUTE(app, "/api/do_authenticated")
      .methods(crow::HTTPMethod::Post, crow::HTTPMethod::Get)
      .CROW_LOGIN_REQUIRED(app)
          ([]() {

            CROW_LOG_INFO << "do authenticated after successful authentication";
            return crow::response(crow::status::OK);
          });

  app.port(18080).run();

}
