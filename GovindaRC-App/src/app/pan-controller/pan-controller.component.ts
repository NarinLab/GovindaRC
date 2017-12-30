import { Component, OnInit, AfterViewInit } from '@angular/core';
import * as nipplejs from 'nipplejs';
import { Subscription } from 'rxjs/Subscription';
import { ServerSocketService } from '../server-socket.service';


@Component({
  selector: 'app-pan-controller',
  templateUrl: './pan-controller.component.html',
  styleUrls: ['./pan-controller.component.css']
})

export class PanControllerComponent implements OnInit {
  private controller_left = null;
  private controller_right = null;
  progress_pwm = 0;
  public progress_mode = "determinate";
  private options = {
      zone: null,                  // active zone
      color: '#FFF',
      size: 150,
      threshold: 0.1,               // before triggering a directional event
      fadeTime: 500,              // transition time
      multitouch: false,
      maxNumberOfNipples: 1,     // when multitouch, what is too many?
      dataOnly: false,              // no dom element whatsoever
      position: {top: '140px', right: '160px'},               // preset position for 'static' mode
      mode: 'static',                   // 'dynamic', 'static' or 'semi'
      restOpacity: 0.5,            // opacity when not 'dynamic' and rested
      catchDistance: 200           // distance to recycle previous joystick in 'semi' mode
  };
  private socketSubscription: Subscription;
  constructor(private socket: ServerSocketService) { }

  ngOnInit(){
    if(this.socket.messages){

    }
  }

  ngAfterViewInit() {
    // Component views are initialized
    this.options.zone = document.getElementById('pan-controller-right');
    this.controller_right = nipplejs.create(this.options);;
    this.controller_right.on('move', this.controllerRightOnMove.bind(this));
    this.controller_right.on('end', this.controllerRightOnEnd.bind(this));
  }

  getDirection(dir){
    switch(dir){
      case 'up': {
        return '1';
      }
      case 'down': {
        return '2';
      }
      case 'left': {
        return '4';
      }
      case 'right': {
        return '3';
      }
      default: {
        return '0';
      }
    }
  }

  getGas(rad, force){
    if(force > 1){
      force = 1;
    }
    if(rad > 0.80 && rad < 1.4){
      //turn right
      let gas_1_percent = (rad - 0.8) / 0.6 * 100;
      let gas_1_pwm_a = ( 1023 * gas_1_percent / 100 );
      let gas_1_pwm_b = (1023 * force);
      //console.log([rad, force, gas_1_percent, gas_1_pwm_a, gas_1_pwm_b]);
      return {a: gas_1_pwm_a, b: gas_1_pwm_b};
    }
    else if(rad > 1.70 && rad < 2.3){
      //turn left
      let gas_2_percent = (2.3 - rad) / 0.6 * 100;
      let gas_2_pwm_a = (1023 * force)
      let gas_2_pwm_b = (1023 * gas_2_percent / 100 );
      //console.log([rad, force, gas_2_percent, gas_2_pwm_a, gas_2_pwm_b]);
      return {a: gas_2_pwm_a, b: gas_2_pwm_b};
    }
    else{
      //gas normal
      return {a: 1023 * force, b: 1023 * force};
    }
  }

  controllerRightOnMove(evt, nipple){
    //console.log(nipple.force);
    //console.log(nipple.angle.radian);
    if(this.socket.messages){
      this.progress_mode = "determinate";
      this.progress_pwm = Math.round(nipple.force * 100);
      let pwm = this.getGas(nipple.angle.radian, nipple.force);
      this.socket.send('{"cmd": "speed", "arg": {"a": '+pwm.a+', "b": '+pwm.b+'}}}');
      this.socket.send('{"cmd": "move", "arg": '+this.getDirection(nipple.direction ? nipple.direction.angle : 'stop')+'}');
    }
  }
  controllerRightOnEnd(evt, nipple){
    if(this.socket.messages){
      this.progress_mode = "indeterminate";
      this.socket.send('{"cmd": "move", "arg": 0}');
    }
  }

  ngOnDestroy() {
    this.socketSubscription.unsubscribe();
  }

  
}