
  static vec3 centerOfPlatform(20,20,0);
  static vec3 leftOfBoss(20,20,-300);
  static vec3 frontOfBoss(20,20,0);
  static vec3 rightOfBoss(20,20,300);

  }
    if(beam && beamKillTimer.Get() > 0.5f)
    {
      beam->destroy();
      beam = NULL;      
    }
    else if(beam)
    {
      ModelComponent* beamModel = beam->getComponentT<ModelComponent>();
      vec3 pos = beamModel->GetPos();

      if(pos.z < -50.0f)
        beamModel->Roll(-0.4f);
      else if(pos.z > 50.0f)
        beamModel->Roll(0.4f);

      beamModel->Pitch(-0.5f);
      beamModel->Yaw(0.5f);
    }

    if(!back && abs(curSpot.z - retSpot.z) > 30.0f)
      UpdatePos();
    else 
      back = true;

    if(state == breathing)
      BreathAttack();

  }






  void BossComponent::BreathAttack()
  {
    Rewrite using new script global boss/player pointers  see scriptGlobal.h
    ////////////////////////////////////////////////////////////////////////

     static vec3 screechSpot;
     if(state == idle)
     {
       //vec3 PlayerPos = PLAYER->getComponentT<Body>()->getPos();
 
       SOUND->Play(BIRDSCRRECHTELL);
 
       //if(PlayerPos.z < -200)
       //  spawnSpot = leftOfBoss;
       //else if(PlayerPos.z > 180)
       //  spawnSpot = rightOfBoss;
       //else
       //  spawnSpot = frontOfBoss;
 
       //ModelComponent* model = parent()->getComponentT<ModelComponent>();
 
       //model->ChangeAnimNOW(Text("bird_screech"));
       //breathTimer.StartTimer();
       //state = breathing;
 
       if(!startPosSet)
       {
         Body& body = *parent()->getComponentT<Body>();
         retSpot = body.getPos();
         startPosSet = true;
       }
 
       screechSpot = spawnSpot;
       screechSpot.x = 400;
       screechSpot.y += 200;
 
       moveDir = (screechSpot - retSpot);
       moveDir.Normalize();
     }
     else if(abs(curSpot.z - screechSpot.z) > 30.0f)
     {
       UpdatePos();
     }
     else if(breathTimer.Get() > 1.5f)
     {
       //Create a beam box at one of three locations, and play the corresponding animation
       beam = FACTORY->createObject("BreathBeam");
       Body* beamBody = beam->getComponentT<Body>();
       ModelComponent* beamModel = beam->getComponentT<ModelComponent>();
       beamModel->SetDirection(vec3(-1,0,0));
       beamBody->setPos(spawnSpot);
       actionTimer.StartTimer();
       beamKillTimer.StartTimer();
       state = idle;
       SOUND->Play(SCREECH);
       moveDir = (retSpot - screechSpot);
       moveDir.Normalize();
       back = false;
     }
  }