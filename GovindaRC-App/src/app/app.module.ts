import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { AppRoutingModule }     from './app-routing/app-routing.module';
import { ServerSocketService } from './server-socket.service';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MaterialModule } from './material/material.module';
import { PanControllerComponent } from './pan-controller/pan-controller.component';
import { FlexLayoutModule } from "@angular/flex-layout";
import { HammerGestureConfig, HAMMER_GESTURE_CONFIG } from '@angular/platform-browser';
import { MatProgressSpinnerModule, MatDialogModule } from '@angular/material';
import { MatInputModule, MatDialog, MatDialogRef, MAT_DIALOG_DATA} from '@angular/material';
import { MatSnackBarModule } from "@angular/material";
import { MatChipsModule } from '@angular/material';
import { DialogOptionsComponent } from './dialog-options/dialog-options.component';
import { SnackbarComponent } from './snackbar/snackbar.component';
import {OverlayContainer} from '@angular/cdk/overlay';

export class MyHammerConfig extends HammerGestureConfig  {
  overrides = <any>{
      'swipe': {velocity: 0.3, threshold: 10} // override default settings
  }
}

@NgModule({
  declarations: [
    AppComponent,
    DashboardComponent,
    PanControllerComponent,
    DialogOptionsComponent,
    SnackbarComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    FormsModule,
    ReactiveFormsModule,
    HttpModule,
    MaterialModule,
    BrowserAnimationsModule,
    FlexLayoutModule,
    MatProgressSpinnerModule,
    MatDialogModule,
    MatInputModule,
    MatSnackBarModule,
    MatChipsModule   
  ],
  entryComponents: [
    DialogOptionsComponent,
    SnackbarComponent
  ],
  providers: [
      ServerSocketService, 
      { 
        provide: HAMMER_GESTURE_CONFIG, 
        useClass: MyHammerConfig 
      }
    ],
  bootstrap: [AppComponent]
})
export class AppModule {
  constructor(overlayContainer: OverlayContainer) {
    //overlayContainer.getContainerElement().classList.add('unicorn-dark-theme');
  }
}
