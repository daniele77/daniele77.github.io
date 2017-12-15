---
layout: post
title: "Messages, commands and events"
published: true
category: general
tags: [design, architecture]
---

Differenza tra messaggi e eventi:
- il mittente di un messaggio conosce il destinatario
- il mittente di un evento non conosce i destinatari, si limita a informare il mondo (tipicamente sono gli interessati che si registrano per averne una notifica)

Esempi di scambio messaggi: 
- un oggetto che invoca un metodo di un oggetto di cui conosce la classe concreta.
- un sotto sistema che invia un comando a un'altro sottosistema.
- un processo che invia un segnale posix (es SIGILL) a un altro processo

Esempio di scambio eventi:
- il subject in observer pattern che notifica tutti gli osservatori senza conoscerli
- i delegati (es signals in boost, etc)
- ...

Conseguenze. 
Nello scambio di messaggi ### ???

Nota un messaggio può diventare un evento. Ad esempio, un sottosistema può inviare un messaggio a un broker su cui si registrano altri sottosistemi per tipo evento o categoria etc.
