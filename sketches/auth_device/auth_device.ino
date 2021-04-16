#include <AuthDeviceWrapperImpl.h>
#include <commproto/authdevice/AuthDevice.h>

commproto::authdevice::AuthDeviceWrapperImpl realDevice;
commproto::authdevice::AuthDevice device(realDevice);

void setup() {
  realDevice.setup();
  device.setup();
}

void loop() {
  device.loop();
}
