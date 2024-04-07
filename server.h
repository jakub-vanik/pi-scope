#pragma once

#include "camera/camera.h"
#include "config_storage.h"
#include "motor.h"
#include "resource_storage.h"
#include "timelapse.h"

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <cstdbool>

class Server : public Poco::Net::HTTPServer
{
public:
  Server(camera::Camera &camera, Motor &motor, Timelapse &timelapse);

private:
  class Handler : public Poco::Net::HTTPRequestHandler
  {
  public:
    Handler(Server &server);
    void handleRequest(Poco::Net::HTTPServerRequest &request,
                       Poco::Net::HTTPServerResponse &response);

  private:
    Server &server;
  };
  class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
  {
  public:
    HandlerFactory(Server &server);
    Poco::Net::HTTPRequestHandler *
    createRequestHandler(const Poco::Net::HTTPServerRequest &request);

  private:
    Server &server;
  };
  typedef bool (Server::*handler)(Poco::Net::HTTPServerRequest &,
                                  Poco::Net::HTTPServerResponse &);
  camera::Camera &camera;
  Motor &motor;
  Timelapse &timelapse;
  ConfigStorage config_storage;
  ResourceStorage resource_storage;
  std::array<handler, 14> handlers;
  void handleRequest(Poco::Net::HTTPServerRequest &request,
                     Poco::Net::HTTPServerResponse &response);
  bool handleImage(Poco::Net::HTTPServerRequest &request,
                   Poco::Net::HTTPServerResponse &response);
  bool handlePreview(Poco::Net::HTTPServerRequest &request,
                     Poco::Net::HTTPServerResponse &response);
  bool handleCameraSetup(Poco::Net::HTTPServerRequest &request,
                         Poco::Net::HTTPServerResponse &response);
  bool handleZoom(Poco::Net::HTTPServerRequest &request,
                  Poco::Net::HTTPServerResponse &response);
  bool handleCalibrate(Poco::Net::HTTPServerRequest &request,
                       Poco::Net::HTTPServerResponse &response);
  bool handleRelativeMove(Poco::Net::HTTPServerRequest &request,
                          Poco::Net::HTTPServerResponse &response);
  bool handleAbsoluteMove(Poco::Net::HTTPServerRequest &request,
                          Poco::Net::HTTPServerResponse &response);
  bool handleRelease(Poco::Net::HTTPServerRequest &request,
                     Poco::Net::HTTPServerResponse &response);
  bool handleFocusBracket(Poco::Net::HTTPServerRequest &request,
                          Poco::Net::HTTPServerResponse &response);
  bool handleStackedFocusBracket(Poco::Net::HTTPServerRequest &request,
                                 Poco::Net::HTTPServerResponse &response);
  bool handleGetTimelapseConfig(Poco::Net::HTTPServerRequest &request,
                                Poco::Net::HTTPServerResponse &response);
  bool handleStartTimelapse(Poco::Net::HTTPServerRequest &request,
                            Poco::Net::HTTPServerResponse &response);
  bool handleStopTimelapse(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);
  bool handleStaticFile(Poco::Net::HTTPServerRequest &request,
                        Poco::Net::HTTPServerResponse &response);
};
