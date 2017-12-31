import { Component, OnInit, Inject } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { ServerSocketService } from './server-socket.service';
import {MatDialog, MatDialogRef, MAT_DIALOG_DATA, MatSnackBarConfig, MatSnackBarModule} from '@angular/material';
import {MatSnackBar, MAT_SNACK_BAR_DATA} from '@angular/material';
import { MatChipsModule } from '@angular/material';
import { DialogOptionsComponent } from './dialog-options/dialog-options.component';
import { SnackbarComponent } from './snackbar/snackbar.component';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'GovindaRC';
  public ultrasonic = 0;
  public isConnected = 'gps_not_fixed';
  public ipAddress = '192.168.43.124';
  public wifiSsid = '-';
  public wifiPass = '-';
  private socketSubscription: Subscription;

  constructor(
    private socket: ServerSocketService, 
    public dialog: MatDialog, 
    public snackbar: MatSnackBar
    ) {}

  ngOnInit() {

  }

  openDialogOptions(): void {
    let dialogRef = this.dialog.open(DialogOptionsComponent, {
      width: '450px',
      data: { ipAddress: this.ipAddress, wifiSsid: this.wifiSsid, wifiPass: this.wifiPass }
    });

    dialogRef.afterClosed().subscribe(data => {
      try{
        this.ipAddress = data.ipAddress;
        this.wifiSsid = data.wifiSsid;
        this.wifiPass = data.wifiPass;
        this.socket.send('{"cmd": "set_wifi", "arg": {"ssid": "'+this.wifiSsid+'", "pass": "'+this.wifiPass+'"}}');
      } catch(err) {}
    });
  }

  openAbout(): void{
    let mySnackBar = this.snackbar.open('GovindaRC App v0.1.1-alpha','OK', {
      duration: 15000,
      data: {message: 'GovindaRC App v0.1.1-alpha', action: 'OK'},
      verticalPosition: 'top',
      horizontalPosition: 'center',
      panelClass: 'snackbar'
    });

    
  }

  websocketConnect(){
    this.socket.ipAddress = this.ipAddress;
    this.socket.connect();
    this.socketSubscription = this.socket.messages.subscribe((message: string) => {
      try {
        var obj = JSON.parse(message);
      } catch (e) {
        obj = {};
      }
      this.isConnected = 'gps_fixed';
      if(obj.type == 'scan'){
        this.ultrasonic = Math.round(obj.data);
      }
      else if(obj.type == 'get_wifi'){
        this.wifiSsid = obj.data.ssid;
        this.wifiPass = obj.data.pass;
      }
    });
    this.socket.messages.retryWhen(errors => errors.delay(1000)).subscribe(message => {
      this.isConnected = 'gps_fixed';
    });
    this.socket.send('{"cmd": "P"}');
    this.socket.send('{"cmd": "get_wifi"}');
  }

  ngOnDestroy() {
    this.socketSubscription.unsubscribe();
    this.isConnected = "gps_not_fixed";
  }

  btnConnectClick(){
    this.websocketConnect();
  }

  btnEyeClick(){
    this.socket.send('{"cmd": "scan", "arg": 0}');
  }
}
