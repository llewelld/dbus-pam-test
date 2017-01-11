# dbus-pam-test
Test the use of DBUS with PAM

## Important files

The program
```
/usr/share/flypig-test/cont
/usr/share/flypig-test/button-11.wav
```

Policy to allow use of system dbus

```
/etc/dbus-1/system.d/uk.co.flypig.test.conf
```

Systemd unit config
```
/etc/systemd/system/flypig-test.service
```

Upstart service config (untested)
```
/etc/init/??.conf
```

## Useful systemd commands:

Show log
```
sudo journalctl -u flypig-test
```

Check status
```
systemctl status flypig-test.service
ps aux | grep /cont
```

Start, stop, reload, enable, disable
```
systemctl start flypig-test.service
systemctl stop flypig-test.service
systemctl daemon-reload
systemctl enable flypig-test.service
systemctl disable flypig-test.service
```

Find out details of the service dbus
```
gdbus introspect --system --dest uk.co.flypig.test --object-path /TestObject
```

Test PAM module
```
pamtester flypig-test $USER authenticate
```

## Credits

Beep sound by distillerystudio (CC attribution).

https://www.freesound.org/people/distillerystudio/sounds/327734/
https://creativecommons.org/licenses/by/3.0/


