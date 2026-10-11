# Makes the PlatformIO "Upload Filesystem Image OTA" task (uploadfsota) send the
# LittleFS image to the turret's /update?target=fs endpoint (see src/web/Ota.cpp).
# Regular uploads (USB) are untouched. The address is `custom_ota_host`.
Import("env")
from SCons.Script import COMMAND_LINE_TARGETS

if "uploadfsota" in COMMAND_LINE_TARGETS:
    host = env.GetProjectOption("custom_ota_host", "192.168.4.1")
    env.Replace(
        UPLOAD_PROTOCOL="custom",
        UPLOADCMD='curl --fail --max-time 300 -F "filesystem=@$SOURCE" '
                  '"http://%s/update?target=fs"' % host,
    )
