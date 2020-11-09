CREATE VIEW public.question1 AS
 SELECT c.first_name,
    c.last_name
   FROM ((public.customer c
     JOIN public.transaction t ON (((c.customer_id)::text = (t.customer_id)::text)))
     JOIN public.purchases p ON (((p.trans_id)::text = (t.trans_id)::text)));

CREATE VIEW public.question2 AS
 SELECT c.email
   FROM (((public.customer c
     JOIN public.transaction t ON (((t.customer_id)::text = (c.customer_id)::text)))
     JOIN public.member m ON (((m.trans_id)::text = (t.trans_id)::text)))
     JOIN public.instructs i ON (((i.fit_id)::text = (m.fit_id)::text)));

CREATE VIEW public.question3 AS
 SELECT t.transaction_date
   FROM public.transaction t
  WHERE (EXISTS ( SELECT p.merch_type
           FROM public.purchases p
          WHERE (((p.trans_id)::text = (t.trans_id)::text) AND (p.quantity > 4))));

CREATE VIEW public.question4 AS
 SELECT m.fit_id,
    m.end_date,
    m.remaining_time
   FROM (public.member m
     JOIN public.membership_type mt ON ((((m.name)::text = (mt.name)::text) AND (mt.pool_access = true))))
  GROUP BY m.fit_id, m.end_date, m.remaining_time
  ORDER BY m.remaining_time;

CREATE VIEW public.question5 AS
 SELECT purchases.quantity,
    purchases.merch_type
   FROM public.purchases
  WHERE (purchases.quantity < ALL ( SELECT merchandise.price
           FROM public.merchandise));

CREATE VIEW public.question6 AS
 SELECT c.first_name,
    p.merch_type
   FROM (((public.customer c
     JOIN public.transaction t ON (((t.customer_id)::text = (c.customer_id)::text)))
     JOIN public.purchases p ON (((p.trans_id)::text = (t.trans_id)::text)))
     JOIN public.merchandise m ON (((m.merch_type)::text = (p.merch_type)::text)))
  WHERE (((t.payment_method)::text = 'visa'::text) AND (m.price > 15));

CREATE VIEW public.question7 AS
 SELECT count(i.fit_id) AS count
   FROM ((public.instructs i
     JOIN public.additional_service a ON (((i.add_service_id)::text = (a.add_service_id)::text)))
     JOIN public.camp c ON (((c.add_service_id)::text = (a.add_service_id)::text)))
  WHERE (((a.start_date)::text = '2020-11-17'::text) AND ((c.event_type)::text = 'walking'::text));

CREATE VIEW public.question8 AS
 SELECT enrollment.fit_id
   FROM public.enrollment
EXCEPT (
         SELECT enrollment.fit_id
           FROM public.enrollment
        EXCEPT
         SELECT instructs.fit_id
           FROM public.instructs
);

CREATE VIEW public.question9 AS
 SELECT avg(p.quantity) AS avg
   FROM (public.purchases p
     JOIN public.transaction t ON (((t.trans_id)::text = (p.trans_id)::text)))
  WHERE ((t.transaction_date > '2017-03-01'::date) AND ((p.merch_type)::text = 'towel'::text));

CREATE VIEW public.question10 AS
 SELECT max(p.quantity) AS max
   FROM (public.purchases p
     JOIN public.member m ON (((m.trans_id)::text = (p.trans_id)::text)))
  GROUP BY m.trans_id;
