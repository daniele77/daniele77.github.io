---
layout: post
title: "Web based UIs for desktop applications"
published: true
category: general
tags: [architecture,C++]
---

- Sono principalmente uno sviluppatore C++ e non sono mai stato pienamente soddisfatto delle soluzioni disponibili per le Ui.
applicazioni desktop, ma anche cruscotti, sistemi chiosco, etc.
- qt e wx sono dei baracconi e È INUTILE: non consentono design belli come quelli a cui siamo abituati dal web.
- SOLUZIONE : (non sempre) APPLICAZIONE web locale + web socket + server locale, il tutto impacchettato in maniera furba.
- questo vale anche per gli altri linguaggi, specialmente wuelli di script che gestiscono websocket ma sono poveri di widget Ui.
- vantaggio ulteriore: siamo costretti a dividere bene tra Ui e logica applicativa.
- non va sempre bene.
