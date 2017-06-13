---
layout: post
title: "FSMs"
published: true
category: general
tags: [design]
---

* Spesso certe applicazioni (reactive) hanno due (o più) flussi di I/O e devono limitarsi 
  a tradurre in qualche modo gli I da una parte in O dall'altra
* Una soluzione (tipica) è quella di avere le parti di I/O "abbastanza ad oggetti", con layer di disaccoppiamento
  e in mezzo una unica classe implementata come FSM
* Ora: [quasi] ogni classe è tecnicamente una FSM (finitezza della RAM), ma quali sono i vantaggi e gli svantaggi
  di implementare ciò che sta in mezzo esplicitamente come FSM?
* Vantaggi: semplicità, formalismo, più "matematico", non richiede grossi sforzi di design
* Svantaggi: controller monolitico, con tutto quello che ne deriva. Per cambiare il comportamento dell'applicazione, come faccio?
