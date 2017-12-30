import { Component, OnInit } from '@angular/core';
import { Subscription } from 'rxjs/Subscription';
import { ServerSocketService } from '../server-socket.service';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {
  private socketSubscription: Subscription;

  constructor(private socket: ServerSocketService) { }

  ngOnInit() {

  }

  
}
