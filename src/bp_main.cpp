#include "crow/http_response.h"
#include "crow/json.h"
#include "crow.h"

#include "model.hpp"
#include "auth.hpp"

Model model;

crow::json::wvalue error2json(const std::string &errstring) {
  crow::json::wvalue r;
  r["error"] = errstring;
  return r;
}

#include "bp_user_registration.hpp"

int main() {
  crow::App<LoginRequiredMiddleware> app;
  bp_user_registration bpUserRegistration;

  app.register_blueprint(bpUserRegistration);

  app.port(18080).multithreaded().run();

}
