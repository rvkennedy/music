#ifndef CONNECT_H
#define CONNECT_H
#define CONNECT_AUTO(a,b,c,d) {bool r=QObject::connect(a,b,c,d);if(!r) throw std::runtime_error("Failed to connect signal/slot.");}
#define CONNECT(a,b,c,d) {bool r=QObject::connect(a,SIGNAL(b),c,SLOT(d));if(!r) throw std::runtime_error("Failed to connect signal/slot.");}
#define CT3(a,b,c,d) CONNECT(a,SIGNAL(c),b,SLOT(c),d)
#define CT3_AUTO(a,b,c) CONNECT_AUTO(a,SIGNAL(c),b,SLOT(c))
#define CT_SIGNALS(a,b,c) CONNECT_AUTO(a,SIGNAL(c),b,SIGNAL(c))
#define CT_SLOTS(a,b,c) CONNECT_AUTO(a,SLOT(c),b,SLOT(c))
#define CT4(a,b,c,d,e) CONNECT(a,SIGNAL(b),c,SLOT(d),e)
#define CT4_AUTO(a,b,c,d) CONNECT_AUTO(a,SIGNAL(b),c,SLOT(d))
#define CT_SIGNALS4(a,b,c,d) CONNECT_AUTO(a,SIGNAL(b),c,SIGNAL(d))
#endif