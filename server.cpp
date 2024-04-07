#include "server.h"

#include "focus_bracket.h"

#include <Poco/JSON/Object.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/MultipartWriter.h>
#include <algorithm>
#include <exception>
#include <regex>
#include <string>
#include <tuple>

extern const char _binary_web_interface_zip_start;
extern const char _binary_web_interface_zip_end;

Server::Server(camera::Camera &camera, Motor &motor, Timelapse &timelapse) :
  Poco::Net::HTTPServer(new HandlerFactory(*this), 8080),
  camera(camera),
  motor(motor),
  timelapse(timelapse),
  config_storage("settings.xml"),
  resource_storage(&_binary_web_interface_zip_start,
                   &_binary_web_interface_zip_end),
  handlers{&Server::handleImage,          &Server::handlePreview,
           &Server::handleCameraSetup,    &Server::handleZoom,
           &Server::handleCalibrate,      &Server::handleRelativeMove,
           &Server::handleAbsoluteMove,   &Server::handleRelease,
           &Server::handleFocusBracket,   &Server::handleStackedFocusBracket,
           &Server::handleStartTimelapse, &Server::handleGetTimelapseConfig,
           &Server::handleStopTimelapse,  &Server::handleStaticFile}
{
  auto camera_settings = config_storage.getCameraSettings();
  camera.setExposureMode(
      MMAL_PARAM_EXPOSUREMODE_T(camera_settings.exposure_mode));
  camera.setExposureMeteringMode(MMAL_PARAM_EXPOSUREMETERINGMODE_T(
      camera_settings.exposure_metering_mode));
  camera.setExposureCompensation(camera_settings.exposure_compensation);
  camera.setAwbMode(MMAL_PARAM_AWBMODE_T(camera_settings.awb_mode));
  camera.setAwbGains(
      {camera_settings.awb_gain_red, camera_settings.awb_gain_blue});
  camera.setIso(camera_settings.iso);
  camera.setShutterSpeed(camera_settings.shutter_speed);
}

Server::Handler::Handler(Server &server) : server(server) {}

void Server::Handler::handleRequest(Poco::Net::HTTPServerRequest &request,
                                    Poco::Net::HTTPServerResponse &response)
{
  server.handleRequest(request, response);
}

Server::HandlerFactory::HandlerFactory(Server &server) : server(server) {}

Poco::Net::HTTPRequestHandler *Server::HandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest &request)
{
  return new Handler(server);
}

void Server::handleRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
{
  try {
    for (auto handler : handlers) {
      if ((*this.*handler)(request, response)) {
        return;
      }
    }
    response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
    response.setContentType("text/plain");
    std::ostream &stream = response.send();
    stream << "\n";
  } catch (const std::exception &ex) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    response.setContentType("text/plain");
    std::ostream &stream = response.send();
    stream << ex.what() << "\n";
  }
}

bool Server::handleImage(Poco::Net::HTTPServerRequest &request,
                         Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/image/([0-9]+)x([0-9]+)/image.jpg");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    unsigned int width = atoi(match[1].first);
    unsigned int height = atoi(match[2].first);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("image/jpeg");
    std::ostream &stream = response.send();
    camera.setZoom({0.0f, 0.0f, 1.0f, 1.0f});
    camera.takePicture(stream, width, height, 100);
    return true;
  }
  return false;
}

bool Server::handlePreview(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/preview/([0-9]+)x([0-9]+)/stream.jpg");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    unsigned int width = atoi(match[1].first);
    unsigned int height = atoi(match[2].first);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    std::string boundary = Poco::Net::MultipartWriter::createBoundary();
    response.setContentType("multipart/x-mixed-replace;boundary=" + boundary);
    Poco::Net::MultipartWriter writer(response.send(), boundary);
    camera.streamPreview(writer, width, height, 90);
    return true;
  }
  return false;
}

bool Server::handleCameraSetup(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/camera_setup\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    Poco::Net::HTMLForm form(request, request.stream());
    auto camera_settings = config_storage.getCameraSettings();
    if (form.has("exposure_mode")) {
      unsigned int exposure_mode = std::stoul(form.get("exposure_mode"));
      camera.setExposureMode(MMAL_PARAM_EXPOSUREMODE_T(exposure_mode));
      camera_settings.exposure_mode = exposure_mode;
    }
    if (form.has("exposure_metering_mode")) {
      unsigned int exposure_metering_mode =
          std::stoul(form.get("exposure_metering_mode"));
      camera.setExposureMeteringMode(
          MMAL_PARAM_EXPOSUREMETERINGMODE_T(exposure_metering_mode));
      camera_settings.exposure_metering_mode = exposure_metering_mode;
    }
    if (form.has("exposure_compensation")) {
      int exposure_compensation = std::stoi(form.get("exposure_compensation"));
      camera.setExposureCompensation(exposure_compensation);
      camera_settings.exposure_compensation = exposure_compensation;
    }
    if (form.has("awb_mode")) {
      unsigned int awb_mode = std::stoul(form.get("awb_mode"));
      camera.setAwbMode(MMAL_PARAM_AWBMODE_T(awb_mode));
      camera_settings.awb_mode = awb_mode;
    }
    if (form.has("awb_gain_r")) {
      float awb_gain_red = std::stof(form.get("awb_gain_r"));
      auto awb_gains = camera.getAwbGains();
      std::get<0>(awb_gains) = awb_gain_red;
      camera.setAwbGains(awb_gains);
      camera_settings.awb_gain_red = awb_gain_red;
    }
    if (form.has("awb_gain_b")) {
      float awb_gain_blue = std::stof(form.get("awb_gain_b"));
      auto awb_gains = camera.getAwbGains();
      std::get<1>(awb_gains) = awb_gain_blue;
      camera.setAwbGains(awb_gains);
      camera_settings.awb_gain_blue = awb_gain_blue;
    }
    if (form.has("iso")) {
      unsigned int iso = std::stoul(form.get("iso"));
      camera.setIso(iso);
      camera_settings.iso = iso;
    }
    if (form.has("shutter_speed")) {
      unsigned int shutter_speed = std::stoul(form.get("shutter_speed"));
      camera.setShutterSpeed(shutter_speed);
      camera_settings.shutter_speed = shutter_speed;
    }
    config_storage.setCameraSettings(camera_settings);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    Poco::JSON::Object setup;
    setup.set("exposure_mode", int(camera.getExposureMode()));
    setup.set("exposure_metering_mode", int(camera.getExposureMeteringMode()));
    setup.set("exposure_compensation", camera.getExposureCompensation());
    setup.set("awb_mode", int(camera.getAwbMode()));
    auto awb_gains = camera.getAwbGains();
    setup.set("awb_gain_r", std::get<0>(awb_gains));
    setup.set("awb_gain_b", std::get<1>(awb_gains));
    setup.set("iso", camera.getIso());
    setup.set("shutter_speed", camera.getShutterSpeed());
    root.set("setup", setup);
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleZoom(Poco::Net::HTTPServerRequest &request,
                        Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/zoom\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    Poco::Net::HTMLForm form(request, request.stream());
    unsigned int in = std::stoul(form.get("in"));
    auto zoom = camera.getZoom();
    float &left = std::get<0>(zoom);
    float &top = std::get<1>(zoom);
    float &width = std::get<2>(zoom);
    float &height = std::get<3>(zoom);
    if (in == 0) {
      left -= width * 0.5f;
      top -= height * 0.5f;
      width *= 2;
      height *= 2;
    } else if (in == 1) {
      float x = std::stof(form.get("x"));
      float y = std::stof(form.get("y"));
      left += width * (x - 0.25f);
      top += height * (y - 0.25f);
      width /= 2;
      height /= 2;
    } else {
      throw std::exception();
    }
    width = std::min(width, 1.0f);
    height = std::min(height, 1.0f);
    left = std::min(std::max(left, 0.0f), 1.0f - width);
    top = std::min(std::max(top, 0.0f), 1.0f - height);
    camera.setZoom(zoom);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleCalibrate(Poco::Net::HTTPServerRequest &request,
                             Poco::Net::HTTPServerResponse &response)

{
  std::regex expression("/calibrate\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    Poco::Net::HTMLForm form(request, request.stream());
    bool direction = std::stoul(form.get("direction"));
    unsigned int limit = std::stoul(form.get("limit"));
    bool success = motor.calibrate(direction, limit);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.set("success", success);
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleRelativeMove(Poco::Net::HTTPServerRequest &request,
                                Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/relative_move\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    Poco::Net::HTMLForm form(request, request.stream());
    int increment = std::stoi(form.get("increment"));
    int position = motor.relativeMove(increment);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.set("position", position);
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleAbsoluteMove(Poco::Net::HTTPServerRequest &request,
                                Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/absolute_move\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    Poco::Net::HTMLForm form(request, request.stream());
    int position = std::stoi(form.get("position"));
    motor.absoluteMove(position);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.set("position", position);
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleRelease(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/release\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    motor.release();
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleFocusBracket(Poco::Net::HTTPServerRequest &request,
                                Poco::Net::HTTPServerResponse &response)
{
  std::regex expression(
      "/focus_bracket/([0-9]+)x([0-9]+)/([0-9]+)x(-?[0-9]+)\\+(-?[0-9]+)/images.zip");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    unsigned int width = atoi(match[1].first);
    unsigned int height = atoi(match[2].first);
    unsigned int count = atoi(match[3].first);
    int increment = atoi(match[4].first);
    int base = atoi(match[5].first);
    FocusBracket focus_bracket(camera, motor, width, height, count, increment,
                               base, 1.0f, false);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/zip");
    std::ostream &stream = response.send();
    focus_bracket.capture(stream);
    return true;
  }
  return false;
}

bool Server::handleStackedFocusBracket(Poco::Net::HTTPServerRequest &request,
                                       Poco::Net::HTTPServerResponse &response)
{
  std::regex expression(
      "/focus_bracket/([0-9]+)x([0-9]+)/([0-9]+)x(-?[0-9]+)\\+(-?[0-9]+)/stack.jpg");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    unsigned int width = atoi(match[1].first);
    unsigned int height = atoi(match[2].first);
    unsigned int count = atoi(match[3].first);
    int increment = atoi(match[4].first);
    int base = atoi(match[5].first);
    FocusBracket focus_bracket(camera, motor, width, height, count, increment,
                               base, 1.0f, true);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("image/jpeg");
    std::ostream &stream = response.send();
    focus_bracket.capture(stream);
    return true;
  }
  return false;
}

bool Server::handleGetTimelapseConfig(Poco::Net::HTTPServerRequest &request,
                                      Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/timelapse/config\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    Poco::JSON::Object config;
    auto timelapse_settings = config_storage.getTimelapseSettings();
    config.set("path", timelapse_settings.path);
    config.set("seconds", timelapse_settings.seconds);
    config.set("width", timelapse_settings.width);
    config.set("height", timelapse_settings.height);
    config.set("mode", timelapse_settings.mode);
    config.set("count", timelapse_settings.count);
    config.set("increment", timelapse_settings.increment);
    config.set("base", timelapse_settings.base);
    config.set("speed", timelapse_settings.speed);
    config.set("stack", int(timelapse_settings.stack));
    root.set("config", config);
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleStartTimelapse(Poco::Net::HTTPServerRequest &request,
                                  Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/timelapse/start\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    Poco::Net::HTMLForm form(request, request.stream());
    auto timelapse_settings = config_storage.getTimelapseSettings();
    std::string path = form.get("path");
    unsigned int seconds = std::stoul(form.get("seconds"));
    unsigned int width = std::stoul(form.get("width"));
    unsigned int height = std::stoul(form.get("height"));
    unsigned int mode = std::stoul(form.get("mode"));
    if (mode == 0) {
      timelapse.startSingle(path, seconds, width, height);
      timelapse_settings.path = path;
      timelapse_settings.seconds = seconds;
      timelapse_settings.width = width;
      timelapse_settings.height = height;
      timelapse_settings.mode = mode;
    } else if (mode == 1) {
      unsigned int count = std::stoul(form.get("count"));
      int increment = std::stoi(form.get("increment"));
      int base = std::stoi(form.get("base"));
      float speed = std::stof(form.get("speed"));
      bool stack = std::stoul(form.get("stack"));
      timelapse.startFocusBracket(path, seconds, width, height, count,
                                  increment, base, speed, stack);
      timelapse_settings.path = path;
      timelapse_settings.seconds = seconds;
      timelapse_settings.width = width;
      timelapse_settings.height = height;
      timelapse_settings.mode = mode;
      timelapse_settings.count = count;
      timelapse_settings.increment = increment;
      timelapse_settings.base = base;
      timelapse_settings.speed = speed;
      timelapse_settings.stack = stack;
    } else {
      throw std::exception();
    }
    config_storage.setTimelapseSettings(timelapse_settings);
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleStopTimelapse(Poco::Net::HTTPServerRequest &request,
                                 Poco::Net::HTTPServerResponse &response)
{
  std::regex expression("/timelapse/stop\\??.*");
  std::cmatch match;
  if (std::regex_match(request.getURI().c_str(), match, expression)) {
    timelapse.stop();
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");
    std::ostream &stream = response.send();
    Poco::JSON::Object root;
    root.stringify(stream);
    return true;
  }
  return false;
}

bool Server::handleStaticFile(Poco::Net::HTTPServerRequest &request,
                              Poco::Net::HTTPServerResponse &response)
{
  std::string uri = request.getURI();
  if (uri == "/") {
    uri = "/index.html";
  }
  return resource_storage.loadResource("web_interface" + uri, response.send());
}
