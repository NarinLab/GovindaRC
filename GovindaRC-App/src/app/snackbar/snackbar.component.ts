import { Component, OnInit, Inject } from '@angular/core';
import {MatSnackBar, MAT_SNACK_BAR_DATA} from '@angular/material';

@Component({
  selector: 'app-snackbar',
  templateUrl: './snackbar.component.html',
  styleUrls: ['./snackbar.component.css']
})
export class SnackbarComponent implements OnInit {
  public message = 'What?';

  constructor(public snackBar: MatSnackBar, @Inject(MAT_SNACK_BAR_DATA) public data: any) {
    
  }

  ngOnInit() {
  }

}
