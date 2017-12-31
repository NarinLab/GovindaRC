import { Component, OnInit, Inject } from '@angular/core';
import {MatInputModule, MatDialog, MatDialogRef, MAT_DIALOG_DATA} from '@angular/material';

@Component({
  selector: 'app-dialog-options',
  template: `
    <h1 mat-dialog-title class="primary-color">Pengaturan</h1>
    <mat-dialog-content class="dialog-warn">
      <form class="config-form">
        
        <mat-form-field class="field-full-width">
          <input matInput name="ipAddress" [(ngModel)]="data.ipAddress" placeholder="IP Address" value="">
          <mat-hint>Alamat IP robot, check dari pengaturan tethering</mat-hint>
        </mat-form-field>
        <p></p>
        <mat-form-field class="field-full-width">
          <input matInput name="wifiSsid" [(ngModel)]="data.wifiSsid" placeholder="WiFi SSID" value="">
          <mat-hint>Nama WiFi (8-30 karakter huruf/angka), robot perlu restart setelah di set!</mat-hint>
        </mat-form-field>
        <p></p>
        <mat-form-field class="field-full-width">
          <input matInput name="wifiPass" [(ngModel)]="data.wifiPass" placeholder="WiFi Password" value="">
          <mat-hint>Password WiFi (8-30 karakter huruf/angka)</mat-hint>
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
    @Inject(MAT_DIALOG_DATA) public data: any) {
     }

  onNoClick(): void {
    this.dialogRef.close();
  }

  ngOnInit() {
  }

}