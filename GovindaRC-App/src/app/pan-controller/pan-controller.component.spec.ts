import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { PanControllerComponent } from './pan-controller.component';

describe('PanControllerComponent', () => {
  let component: PanControllerComponent;
  let fixture: ComponentFixture<PanControllerComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ PanControllerComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(PanControllerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
