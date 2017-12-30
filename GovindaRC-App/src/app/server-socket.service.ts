import { Injectable } from '@angular/core';
import { QueueingSubject } from 'queueing-subject';
import { Observable } from 'rxjs/Observable';
import 'rxjs/add/operator/share';
import 'rxjs/add/operator/retryWhen';
import 'rxjs/add/operator/delay';
import  websocketConnect , { Connection } from 'rxjs-websockets';
 
@Injectable()
export class ServerSocketService {
  private inputStream: QueueingSubject<string>;
  public messages: Observable<any>;
  public ipAddress = '192.168.43.100';
  private lastIpAddress = '';
  public ledState = 1;

  public connect() {
    if (this.messages && this.ipAddress == this.lastIpAddress){
      this.send('{"cmd": "light", "arg": '+this.ledState+'}');
      if(this.ledState == 1){
        this.ledState = 0;
      }
      else{
        this.ledState = 1;
      }
      return;
    }
    this.lastIpAddress = this.ipAddress;
    // Using share() causes a single websocket to be created when the first
    // observer subscribes. This socket is shared with subsequent observers
    // and closed when the observer count falls to zero.
    this.messages = websocketConnect(
      'ws://'+this.ipAddress+':81/ws',
      this.inputStream = new QueueingSubject<string>(),
      'GovindaRC'
    ).messages.share();
  }

  public send(message: string):void {
    // If the websocket is not connected then the QueueingSubject will ensure
    // that messages are queued and delivered when the websocket reconnects.
    // A regular Subject can be used to discard messages sent when the websocket
    // is disconnected.
    if(this.messages){
      this.inputStream.next(message)
    }
  }
}