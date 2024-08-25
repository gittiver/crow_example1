//
// Created by Gulliver on 12.08.24.
//

#ifndef CROW_AUTH_HPP
#define CROW_AUTH_HPP

#include <crow/middleware.h>
#include <crow/http_response.h>

#include <optional>

struct AuthContext {
  std::optional<std::string> username;
};
struct LoginRequiredMiddleware : crow::ILocalMiddleware {
  using context = AuthContext;

  void before_handle(crow::request &req, crow::response &res, context &ctx) const;

  void after_handle(crow::request &req, crow::response &res, context &ctx) const;
};

#define CROW_LOGIN_REQUIRED(app) CROW_MIDDLEWARES(app, LoginRequiredMiddleware)

// End of Authentication utilities

#endif //CROW_AUTH_HPP
