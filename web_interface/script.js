function loadPosition() {
  var http = new XMLHttpRequest();
  http.open("POST", "relative_move", true);
  http.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.response);
      document.getElementById("position_input").value = data["position"];
    }
  };
  var data = new FormData();
  data.append("increment", 0);
  http.send(data);
}

function loadCameraSetup() {
  var http = new XMLHttpRequest();
  http.open("GET", "camera_setup", true);
  http.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.response);
      for (var [key, value] of Object.entries(data["setup"])) {
        document.getElementsByName(key)[0].value = value;
      }
    }
  };
  http.send();
}

function load() {
  loadPosition();
  loadCameraSetup();
}

function setupZoom(element) {
  function handler(event) {
    event.preventDefault();
    var rect = element.getBoundingClientRect();
    var http = new XMLHttpRequest();
    http.open("POST", "zoom", true);
    var data = new FormData();
    data.append("in", (event.deltaY < 0) ? 1 : 0);
    data.append("x", (event.clientX - rect.left) / (rect.right - rect.left));
    data.append("y", (event.clientY - rect.top) / (rect.bottom - rect.top));
    http.send(data);
  }
  element.addEventListener("mouseout", function () {
    element.removeEventListener("wheel", handler);
  });
  element.addEventListener("wheel", handler);
}

function backgroundSubmit(url, form, callback = null) {
  var http = new XMLHttpRequest();
  http.open("POST", url, true);
  http.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      if (callback) {
        var data = JSON.parse(this.response);
        callback(data);
      }
    }
  };
  var data = new FormData(form);
  http.send(data);
  return false;
}

function calibrate(form) {
  return backgroundSubmit("calibrate", form, function (data) {
    if (data["success"]) {
      document.getElementById("position_input").value = 30000;
      absoluteMove(document.getElementById("absolute_move_form"));
    } else {
      document.getElementById("position_input").value = 0;
    }
  });
}

function absoluteMove(form) {
  return backgroundSubmit("absolute_move", form, function (data) {
    document.getElementById("position_input").value = data["position"];
  });
}

function relativeMove(form) {
  return backgroundSubmit("relative_move", form, function (data) {
    document.getElementById("position_input").value = data["position"];
  });
}

function release(form) {
  return backgroundSubmit("release", form);
}

function updateIncrements(select) {
  document.getElementById("forward_increment_input").value = select.value;
  document.getElementById("backward_increment_input").value = -select.value;
}

function cameraSetup(form) {
  return backgroundSubmit("camera_setup", form);
}

function changeTimelapseMode() {
  var mode = document.getElementById("timelapse_mode").value;
  for (var element of document.getElementsByName("focus_bracket_options")) {
    if (mode == "1") {
      element.style.display = "table-row";
    } else {
      element.style.display = "none";
    }
  }
}

function loadTimelapseConfig() {
  var http = new XMLHttpRequest();
  http.open("GET", "timelapse/config", true);
  http.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.response);
      for (var [key, value] of Object.entries(data["config"])) {
        document.getElementsByName(key)[0].value = value;
      }
      changeTimelapseMode();
    }
  };
  http.send();
}

function startTimelapse(form) {
  return backgroundSubmit("timelapse/start", form, function (data) {
    window.location.href = "/";
  });
}

function stopTimelapse(form) {
  return backgroundSubmit("timelapse/stop", form, function (data) {
    window.location.href = "/";
  });
}
