--
-- PostgreSQL database dump
--

-- Dumped from database version 10.12 (Ubuntu 10.12-0ubuntu0.18.04.1)
-- Dumped by pg_dump version 10.12 (Ubuntu 10.12-0ubuntu0.18.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: additional_service; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.additional_service (
    add_service_id character varying NOT NULL,
    start_date character varying,
    end_date character varying
);


ALTER TABLE public.additional_service OWNER TO voyeuse;

--
-- Name: camp; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.camp (
    add_service_id character varying NOT NULL,
    event_duration character varying,
    training_type character varying,
    event_type character varying
);


ALTER TABLE public.camp OWNER TO voyeuse;

--
-- Name: customer; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.customer (
    customer_id character varying NOT NULL,
    first_name character varying,
    last_name character varying,
    email character varying,
    phone character varying,
    mailing_address character varying
);


ALTER TABLE public.customer OWNER TO voyeuse;

--
-- Name: enrollment; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.enrollment (
    fit_id character varying,
    add_service_id character varying
);


ALTER TABLE public.enrollment OWNER TO voyeuse;

--
-- Name: instructs; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.instructs (
    fit_id character varying,
    add_service_id character varying
);


ALTER TABLE public.instructs OWNER TO voyeuse;

--
-- Name: member; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.member (
    fit_id character varying NOT NULL,
    trans_id character varying,
    name character varying,
    pass_type character varying,
    start_date character varying,
    end_date character varying,
    remaining_time character varying
);


ALTER TABLE public.member OWNER TO voyeuse;

--
-- Name: membership_type; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.membership_type (
    name character varying NOT NULL,
    pool_access boolean,
    gym_access boolean,
    rockwall_access boolean,
    price_factor integer
);


ALTER TABLE public.membership_type OWNER TO voyeuse;

--
-- Name: merchandise; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.merchandise (
    merch_type character varying NOT NULL,
    price integer,
    in_stock boolean
);


ALTER TABLE public.merchandise OWNER TO voyeuse;

--
-- Name: pass; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.pass (
    pass_type character varying NOT NULL,
    price integer
);


ALTER TABLE public.pass OWNER TO voyeuse;

--
-- Name: purchases; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.purchases (
    merch_type character varying,
    trans_id character varying,
    quantity integer
);


ALTER TABLE public.purchases OWNER TO voyeuse;

--
-- Name: transaction; Type: TABLE; Schema: public; Owner: voyeuse
--

CREATE TABLE public.transaction (
    trans_id character varying NOT NULL,
    customer_id character varying,
    transaction_date date,
    payment_method character varying
);


ALTER TABLE public.transaction OWNER TO voyeuse;

--
-- Name: question1; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question1 AS
 SELECT c.first_name,
    c.last_name
   FROM ((public.customer c
     JOIN public.transaction t ON (((c.customer_id)::text = (t.customer_id)::text)))
     JOIN public.purchases p ON (((p.trans_id)::text = (t.trans_id)::text)));


ALTER TABLE public.question1 OWNER TO voyeuse;

--
-- Name: question10; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question10 AS
 SELECT max(p.quantity) AS max
   FROM (public.purchases p
     JOIN public.member m ON (((m.trans_id)::text = (p.trans_id)::text)))
  GROUP BY m.trans_id;


ALTER TABLE public.question10 OWNER TO voyeuse;

--
-- Name: question2; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question2 AS
 SELECT c.email
   FROM (((public.customer c
     JOIN public.transaction t ON (((t.customer_id)::text = (c.customer_id)::text)))
     JOIN public.member m ON (((m.trans_id)::text = (t.trans_id)::text)))
     JOIN public.instructs i ON (((i.fit_id)::text = (m.fit_id)::text)));


ALTER TABLE public.question2 OWNER TO voyeuse;

--
-- Name: question3; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question3 AS
 SELECT t.transaction_date
   FROM public.transaction t
  WHERE (EXISTS ( SELECT p.merch_type
           FROM public.purchases p
          WHERE (((p.trans_id)::text = (t.trans_id)::text) AND (p.quantity > 4))));


ALTER TABLE public.question3 OWNER TO voyeuse;

--
-- Name: question4; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question4 AS
 SELECT m.fit_id,
    m.end_date,
    m.remaining_time
   FROM (public.member m
     JOIN public.membership_type mt ON ((((m.name)::text = (mt.name)::text) AND (mt.pool_access = true))))
  GROUP BY m.fit_id, m.end_date, m.remaining_time
  ORDER BY m.remaining_time;


ALTER TABLE public.question4 OWNER TO voyeuse;

--
-- Name: question5; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question5 AS
 SELECT purchases.quantity,
    purchases.merch_type
   FROM public.purchases
  WHERE (purchases.quantity < ALL ( SELECT merchandise.price
           FROM public.merchandise));


ALTER TABLE public.question5 OWNER TO voyeuse;

--
-- Name: question6; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question6 AS
 SELECT c.first_name,
    p.merch_type
   FROM (((public.customer c
     JOIN public.transaction t ON (((t.customer_id)::text = (c.customer_id)::text)))
     JOIN public.purchases p ON (((p.trans_id)::text = (t.trans_id)::text)))
     JOIN public.merchandise m ON (((m.merch_type)::text = (p.merch_type)::text)))
  WHERE (((t.payment_method)::text = 'visa'::text) AND (m.price > 15));


ALTER TABLE public.question6 OWNER TO voyeuse;

--
-- Name: question7; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question7 AS
 SELECT count(i.fit_id) AS count
   FROM ((public.instructs i
     JOIN public.additional_service a ON (((i.add_service_id)::text = (a.add_service_id)::text)))
     JOIN public.camp c ON (((c.add_service_id)::text = (a.add_service_id)::text)))
  WHERE (((a.start_date)::text = '2020-11-17'::text) AND ((c.event_type)::text = 'walking'::text));


ALTER TABLE public.question7 OWNER TO voyeuse;

--
-- Name: question8; Type: VIEW; Schema: public; Owner: voyeuse
--

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


ALTER TABLE public.question8 OWNER TO voyeuse;

--
-- Name: question9; Type: VIEW; Schema: public; Owner: voyeuse
--

CREATE VIEW public.question9 AS
 SELECT avg(p.quantity) AS avg
   FROM (public.purchases p
     JOIN public.transaction t ON (((t.trans_id)::text = (p.trans_id)::text)))
  WHERE ((t.transaction_date > '2017-03-01'::date) AND ((p.merch_type)::text = 'towel'::text));


ALTER TABLE public.question9 OWNER TO voyeuse;

--
-- Data for Name: additional_service; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.additional_service (add_service_id, start_date, end_date) FROM stdin;
4ab	2020-11-17	2020-12-17
4ac	2020-12-17	2021-01-17
4aa	2020-10-17	2020-11-17
\.


--
-- Data for Name: camp; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.camp (add_service_id, event_duration, training_type, event_type) FROM stdin;
4aa	4 weeks	cardio	running
4ab	2 weeks	conditioning	walking
\.


--
-- Data for Name: customer; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.customer (customer_id, first_name, last_name, email, phone, mailing_address) FROM stdin;
1aa	joe	joey	joe@email.ca	123456789	123 ave
1ab	tim	bro	bro@email.ca	123456756	156 ave
1ac	bob	dude	dude@email.ca	123456766	167 ave
1ad	bp	hans	hans@email.ca	12345674556	14556 ave
1ae	man	man	mans@email.ca	1234456	55556 ave
1af	but	ler	butler@email.ca	12344456	1534546 ave
1ag	so	late	late@email.ca	12389896	1533 ave
1ah	why	dointhis	why@email.ca	1233496	7873 ave
1ai	dis	lic	lic@email.ca	12344456	1534546 ave
1aj	arm	darn	darn@email.ca	12344456	1534546 ave
1ak	barn	barn	darn@email.ca	12344456	1534546 ave
1al	a	a	a@email.ca	12344456	1534546 ave
1am	lu	yup	lastone@email.ca	12334346	153346 ave
\.


--
-- Data for Name: enrollment; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.enrollment (fit_id, add_service_id) FROM stdin;
3ad	4ab
3ae	4ab
3ab	4ab
\.


--
-- Data for Name: instructs; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.instructs (fit_id, add_service_id) FROM stdin;
3ad	4aa
3ae	4aa
3ag	4ab
\.


--
-- Data for Name: member; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.member (fit_id, trans_id, name, pass_type, start_date, end_date, remaining_time) FROM stdin;
3ad	2ad	RA	annual	2020-01-01	2021-01-01	365
3ae	2ae	AA	annual	2020-01-01	2021-01-01	365
3ag	2ag	AA	quarterly	2020-01-01	2020-03-01	90
3af	2af	AA	quarterly	2020-01-01	2020-03-01	90
3ab	2ab	AA	quarterly	2020-01-01	2020-03-01	90
3ai	2ai	PA	annual	2020-01-01	2021-01-01	365
3ak	2ak	PGA	10 day drop-in	2020-01-01	\N	10
3aj	2aj	GA	10 day drop-in	2020-01-01	\N	10
\.


--
-- Data for Name: membership_type; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.membership_type (name, pool_access, gym_access, rockwall_access, price_factor) FROM stdin;
NA	f	f	f	0
PA	t	f	f	5
GA	f	t	f	7
RA	f	f	t	10
PGA	t	t	f	11
PRA	t	f	t	13
GRA	f	t	t	15
AA	t	t	t	20
\.


--
-- Data for Name: merchandise; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.merchandise (merch_type, price, in_stock) FROM stdin;
shirt	30	t
towel	10	t
lock	15	f
\.


--
-- Data for Name: pass; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.pass (pass_type, price) FROM stdin;
Single day	2
quarterly	9
annual	29
10 day drop-in	5
\.


--
-- Data for Name: purchases; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.purchases (merch_type, trans_id, quantity) FROM stdin;
shirt	2aa	3
towel	2ab	4
towel	2ac	10
towel	2af	10
shirt	2ag	19
lock	2ah	4
towel	2al	88
towel	2am	63
\.


--
-- Data for Name: transaction; Type: TABLE DATA; Schema: public; Owner: voyeuse
--

COPY public.transaction (trans_id, customer_id, transaction_date, payment_method) FROM stdin;
2aa	1aa	2017-01-12	visa
2ab	1ab	2017-02-12	cash
2ac	1ac	2017-05-17	pennies
2ad	1ad	2017-09-19	visa
2ae	1ae	2017-09-19	cash
2af	1af	2020-03-12	mastercard
2ag	1ag	2020-04-12	visa
2ah	1ah	2020-04-13	pennies
2ai	1ai	2017-02-12	visa
2aj	1aj	2017-02-12	visa
2ak	1ak	2017-03-12	cash
2al	1al	2017-01-12	visa
2am	1am	2020-01-12	visa
\.


--
-- Name: additional_service additional_service_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.additional_service
    ADD CONSTRAINT additional_service_pkey PRIMARY KEY (add_service_id);


--
-- Name: camp camp_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.camp
    ADD CONSTRAINT camp_pkey PRIMARY KEY (add_service_id);


--
-- Name: customer customer_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.customer
    ADD CONSTRAINT customer_pkey PRIMARY KEY (customer_id);


--
-- Name: member member_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.member
    ADD CONSTRAINT member_pkey PRIMARY KEY (fit_id);


--
-- Name: membership_type membership_type_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.membership_type
    ADD CONSTRAINT membership_type_pkey PRIMARY KEY (name);


--
-- Name: merchandise merchandise_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.merchandise
    ADD CONSTRAINT merchandise_pkey PRIMARY KEY (merch_type);


--
-- Name: pass pass_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.pass
    ADD CONSTRAINT pass_pkey PRIMARY KEY (pass_type);


--
-- Name: transaction transaction_pkey; Type: CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.transaction
    ADD CONSTRAINT transaction_pkey PRIMARY KEY (trans_id);


--
-- Name: camp camp_add_service_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.camp
    ADD CONSTRAINT camp_add_service_id_fkey FOREIGN KEY (add_service_id) REFERENCES public.additional_service(add_service_id);


--
-- Name: enrollment enrollment_add_service_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.enrollment
    ADD CONSTRAINT enrollment_add_service_id_fkey FOREIGN KEY (add_service_id) REFERENCES public.additional_service(add_service_id);


--
-- Name: enrollment enrollment_fit_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.enrollment
    ADD CONSTRAINT enrollment_fit_id_fkey FOREIGN KEY (fit_id) REFERENCES public.member(fit_id);


--
-- Name: instructs instructs_add_service_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.instructs
    ADD CONSTRAINT instructs_add_service_id_fkey FOREIGN KEY (add_service_id) REFERENCES public.additional_service(add_service_id);


--
-- Name: instructs instructs_fit_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.instructs
    ADD CONSTRAINT instructs_fit_id_fkey FOREIGN KEY (fit_id) REFERENCES public.member(fit_id);


--
-- Name: member member_name_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.member
    ADD CONSTRAINT member_name_fkey FOREIGN KEY (name) REFERENCES public.membership_type(name);


--
-- Name: member member_pass_type_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.member
    ADD CONSTRAINT member_pass_type_fkey FOREIGN KEY (pass_type) REFERENCES public.pass(pass_type);


--
-- Name: member member_trans_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.member
    ADD CONSTRAINT member_trans_id_fkey FOREIGN KEY (trans_id) REFERENCES public.transaction(trans_id);


--
-- Name: purchases purchases_merch_type_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.purchases
    ADD CONSTRAINT purchases_merch_type_fkey FOREIGN KEY (merch_type) REFERENCES public.merchandise(merch_type);


--
-- Name: purchases purchases_trans_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.purchases
    ADD CONSTRAINT purchases_trans_id_fkey FOREIGN KEY (trans_id) REFERENCES public.transaction(trans_id);


--
-- Name: transaction transaction_customer_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: voyeuse
--

ALTER TABLE ONLY public.transaction
    ADD CONSTRAINT transaction_customer_id_fkey FOREIGN KEY (customer_id) REFERENCES public.customer(customer_id);


--
-- PostgreSQL database dump complete
--

