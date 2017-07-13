---
layout: post
title: "[smart] abstractions"
published: true
category: general
tags: [architecture,design]
---

- Recentemente ho dovuto progettare un protocollo che consente di parlare tra un server e diversi endpoint tramite json su websocket
- Uno dei desiderata era quello di svincolare l'implementazione dal trasporto (websocket) e possibilmente il formato dati (json)
- Se do la possibilità di usare altri trasporti (es. tcp, udp, ...) devo mettere un'interfaccia di qualche tipo che accomuni tutti i possibili trasporti
- Perdo quindi la possibilità di sfruttare le caratteristiche delle websocket (che è un protocollo connesso, quindi ad es. sapere quando un endpoint si disconnette)
- Tocca mettere qualcosa a livello applicativo (es. ping)
- Vale la pena aprirsi le porte a nuove possibilità rinunciando alle peculiarità della scelta corrente?
- Esempio del protocollo: soluzione 1 (solo websocket), soluzione 2 (interfaccia comune)
- Soluzione 3: due astrazioni: ConnectionTransport e ConnectionLessTransport
- Diagramma UML della soluzione [ok, non tutti lo capiscono, è per quello che non tutti capiscono la OO]
