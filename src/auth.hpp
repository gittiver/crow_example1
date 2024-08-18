//
// Created by Gulliver on 12.08.24.
//

#ifndef CROW_AUTH_HPP
#define CROW_AUTH_HPP

#include <crow/http_response.h>
#include <crow/middleware.h>


#include <optional>

struct AuthContext {
  std::optional<std::string> username;
};

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
    if (is_valid(username, password))
      return true;
    else {
      response.code = crow::status::UNAUTHORIZED;
      response.set_header("WWW-Authenticate",
                          "Basic realm=\"User Visible Realm\", charset=\"UTF-8\"");
      return false;
    }
  }
}

struct LoginRequiredMiddleware : crow::ILocalMiddleware {
  using context = AuthContext;

  void before_handle(crow::request &req, crow::response &res, context &ctx) {
    CROW_LOG_DEBUG << "check auth for " << req.url;
    if (!validate_authentication(req, res)) {
      res.end();
    }
  }

  void after_handle(crow::request &req, crow::response &res, context &ctx) {}
};

#define CROW_LOGIN_REQUIRED(app) CROW_MIDDLEWARES(app, LoginRequiredMiddleware)

// End of Authentication utilities

#endif //CROW_AUTH_HPP
