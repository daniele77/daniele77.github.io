---
layout: post
title: "(C++) decoupling techniques"
published: true
category: general
tags: [C++, design]
---

Occorre esaminare ogni dipendenza da un comportamento concreto che sia scolpita nel software.
In tale situazione, infatti, si hanno diversi potenziali problemi:

- il dipendente non può essere esteso con comportamenti alternativi (in successive versioni del software ma anche
  a run-time in momenti diversi dell’esecuzione dell’applicativo)
- il dipendente non può essere riutilizzato se non con il comportamento concreto
  [se voglio riutilizzare un componente (es classe), devo parametrizzare tutto, compresi i comportamenti]

Ad esempio. Ho un'unica funzione, all'interno della quale ho anche affogato un comportamento concreto.
Al di là del fatto che non posso riutilizzare il comportamento concreto (avrei dovuto metterlo in una funzione o classe),
se in futuro volessi cambiare il comportamento, dovrei modificare la funzione.
Se in futuro volessi usare la funzione anche in un altro posto (che richiede un diverso comportamento) non potrei. [altro ???]
Si noti che sotto l’estendibilità ricade anche la testabilità: se ad esempio voglio testare una classe in isolamento,
è utile modificare i comportamenti dei dipendenti (es. per stimolare / raccogliere i risultati della classe sotto test).

[Occorre tenere in considerazione anche la “modificabilità” andando in aggiunta?]

# Tecniche (C++)

- il comportamento è isolato in un metodo. Il metodo può essere modificato (sorgenti) oppure virtuale e modificato nelle clsssi derivate
- il comportamento è isolato in una classe e l'utilizzatore ha un riferimento a un'istanza della classe. La classe può essere modificata (sorgenti) oppure estesa per ereditarietà
- il comportamento è isolato in una classe utilizzata come parametro template
- il comportamento è isolato in una std::function (membro di una classe o parametro di metodo)
- il client manda un evento e qualcuno si occupa di fare l'azione corrispondente (algoritmo)
- lambda ???

# Elenco delle tecniche (in ordine di disaccoppiamento crescente)

- funzione non virtuale / dipendenza da classe concreta (devo modificare i sorgenti)
- template (devo ricompilare)
- std::function [più adatta al riuso che all’estendibilità]
- metodo virtuale da estendere nelle classi derivate
- classe interfaccia
- evento
- [dove metto le lambda?]

# Analisi delle tecniche

## Funzione non virtuale / dipendenza da classe concreta

Questo significa che il comportamento è fissato definitivamente a tempo di compilazione.
A run-time non è possibile modificare il comportamento (che risiede nella funzione o nella classe concreta).
Non posso riutilizzare la stessa classe nello stesso applicativo a meno che richieda lo stesso identico comportamento (dovrei duplicare il codice della classe modificando la funzione virtuale o la classe concreta usata).
Posso riutilizzare la classe in un’altra applicazione o estendere il comportamento in successive versioni modificando il codice della funzione /classe concreta da cui dipende. Se non altro la modifica è isolata in un unico elemento.

## Template

Una classe A è dipendente da una parte di codice (la include direttamente o fa riferimento ad una funzione o classe concreta che contiene il codice).
Problemi: v. quelli del capitolo “interfaccia”.
Soluzione: il codice da cui dipende A viene fattorizzato in metodi di una classe B, che diventa un parametro template di A.
Questo permette:

- di riutilizzare A all’interno dello stesso applicativo o in altri, senza legarsi al comportamento specifico
- di estendere A in successive versioni dell’applicativo, sostituendo B con C ad esempio, e ricompilando.

Non è invece possibile decidere il comportamento a run-time (cioè modificare il comportamento oppure caricare il comportamento da un file di configurazione).

Vantaggi:

- efficienza (il codice è selezionato a compile-time)
- controllo a compile-time

Svantaggi:

- estendibilità solo ricompilando
- problema della creazione
- l’interfaccia del parametro è implicita
- i comportamenti da parametrizzare per essere raggruppati nella classe parametro dovrebbero rappresentare un’astrazione sensata

## std::function

Come funziona?
Ho una classe che ha un membro std::function es.

```c++
class A
{
public:
    explicit A( const std::function<void()>& _f ) : f( _f ) {}
    void Set( const std::function<void()>& _f ) { f = _f }
    void Foo() { f(); }
private:
    std::function<void()> f;
};

B b;
A a( std::bind( &B::g, &b ) );
a.Foo(); 
C c;
a.Set( std::bind( &C::h, &c ) );
a.Foo();
```

Questo permette:

- di riutilizzare A all’interno dello stesso applicativo o in altri, senza legarsi al comportamento specifico
- di estendere A in successive versioni dell’applicativo, sostituendo B con C ad esempio.

E’ anche possibile decidere il comportamento a run-time (cioè modificare il comportamento oppure caricare il comportamento da un file di configurazione).
Vantaggi:

- estendibilità a run-time
- esplicitazione delle dipendenze di una classe
- interfaccia esplicita, controllo a compile-time
- non richiede di raggruppare più funzioni in un’astrazione sensata

Svantaggi:

- problema della creazione (?)
- lentezza (?)

## Metodo virtuale da estendere nelle classi derivate

Ci sono due casi:

- metodo virtuale pubblico invocato da altre classi
- metodo virtuale privato/protetto invocato solo dalla stessa classe

Il primo caso è equivalente alla classe interfaccia [vero?].

Il secondo caso risolve il problema della riusabilità: posso riutilizzare la classe cambiando il comportamento (subclassando) .Questo richiede però di scrivere una nuova classe derivata.
Risolve anche il problema dell’estendibilità: se ho un puntatore, posso farlo puntare ad una nuova classe che deriva da quella precedente.
[TODO]

Vantaggi:

[TODO]

Svantaggi:

- Richiede di scrivere una nuova classe derivata

[TODO]

## Classe interfaccia

A utilizza B.

Conseguenze:

- Se voglio modificare il comportamento di A, devo modificare i sorgenti di B. Questo significa anche che non c’è modo di modificare il comportamento di A a runtime. Non c’è modo di usare nella stessa applicazione istanze diverse di A che abbiano comportamenti diversi.
- Se voglio riutilizzare A, devo portarmi dietro B (quindi tenendo lo stesso comportamento). Anche se fosse accettabile lo stesso comportamento, B a sua volta si porterà dietro altre classi e così via; risultato: prendere una classe significa prendere tutto il sottoalbero delle dipendenze.

Questo caso copre il test black box della classe.

Soluzione: A utilizza un’interfaccia X implementata da B.
Prendere A significa portarsi dietro X che comunque non ha alcuna dipendenza (ho ridotto il sottoalbero). X può essere implementata da una nuova classe, col comportamento desiderato.
Se voglio modificare il comportamento di A (anche a runtime) devo solo sostituire il riferimento a X con una sua nuova implementazione.

Vantaggi:

- estendibilità a run-time
- esplicitazione delle dipendenze di una classe
- interfaccia esplicita, controllo a compile-time

Svantaggi:

- problema della creazione
- lentezza (?)
- i comportamenti da parametrizzare per essere raggruppati in una classe interfaccia dovrebbero rappresentare un’astrazione sensata

## Eventi

E’ un meccanismo di disaccoppiamento molto potente, ma richiede un’infrastruttura più pesante.
Due classi (o componenti) si parlano mediante lo scambio di eventi. Questo scambio richiede un elemento centrale in grado di inoltrare gli eventi presso chi si è registrato (chiamiamolo event bus).
Se la classe A usa la classe B, è possibile disaccoppiare facendo in modo che A lanci un evento E e B si registri per avere le notifiche dell’evento E.
Una classe emette certi eventi e si sottoscrive per altri.
Riuso in una nuova applicazione. Prendo la classe e la porto così com'è. L'interfaccia è implicita (Quali eventi si attende? Quali emette?). Per funzionare avrà bisogno che qualcun'altra emetta certi eventi. Ok: comunque non devo modificare il codice.
Riuso nella stessa applicazione. Significa avere più istanze della stessa classe nell'applicazione. Non è così banale. [il punto centrale sarebbe parametrizzare il comportamento della classe! mentre gli eventi sembrano più un meccanismo di IO del flusso di dati della classe].
Estensione. Voglio cambiare il comportamento. Significa che devo sostituire un'altra classe che si registra sugli eventi emessi dalla classe in oggetto. Anche a run-time.

[TODO]

Vantaggi:

[TODO]

Svantaggi:

- Infrastruttura pesante
- Interfaccia implicita
- Ogni classe / componente è dipendente dall’event bus
- Richiede di strutturare l’applicazione in maniera asincrona

[TODO]

## Lambda

[TODO]

# Comparazione delle tecniche

| Tecnica       | Modifica comportamento a run-time  | -  |
| ------------- |:-------------:| -----:|
| Funzione non virtuale      | no |  |
| Template | no      |    |
| std::function | yes      |     |
| metodo virtuale | yes      |     |
| classe interfaccia | yes      |    |
| eventi | yes      |     |
| lambda | no      |     |

# Disaccoppiamento (2)

Consideriamo una classe che abbia informazioni da esportare. Quali sono i metodi possibili?

- metodi query (pull)
- meccanismo di sottoscrizione tramite interfaccia / observer (push)
- meccanismo di sottoscrizione tramite delegati (push)
- meccanismo di sottoscrizione tramite eventi / publisher subscriber anonimo (push)

Ma è corretto parlare di “informazioni esportate da una classe”?
Una classe non dovrebbe esportare “comportamenti”? Come si concilia questo con l’esistenza dei metodi precedenti?
