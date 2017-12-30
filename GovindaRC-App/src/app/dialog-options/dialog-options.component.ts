import { Component, OnInit, Inject } from '@angular/core';
import {MatInputModule, MatDialog, MatDialogRef, MAT_DIALOG_DATA} from '@angular/material';
@Component({
  selector: 'app-dialog-options',
  template: `
    <h1 mat-dialog-title class="primary-color">Pengaturan</h1>
    <mat-dialog-content class="dialog-warn">
      <form class="config-form">
        <mat-form-field color="warn" class="field-full-width">
          <input matInput name="ipAddress" [(ngModel)]="data.ipAddress" placeholder="IP Address" value="">
        </mat-form-field>
      </form>
    </mat-dialog-content>
    <mat-dialog-actions>
      <button mat-raised-button color="primary" [mat-dialog-close]="data">
        OK
      </button>  
    </mat-dialog-actions>
  `,
  styles: []
})

export class DialogOptionsComponent implements OnInit {
  constructor(
    public dialogRef: MatDialogRef<DialogOptionsComponent>,
    @Inject(MAT_DIALOG_DATA) public data: any) { }

  onNoClick(): void {
    this.dialogRef.close();
  }

  ngOnInit() {
  }

}
