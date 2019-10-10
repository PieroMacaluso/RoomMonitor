-- PRESENZE RANGE
-- Dati inizio e fine raggruppa in piccoli gruppi di 5 minuti e calcola media in quelli
SELECT timing, COUNT(DISTINCT mac_addr)
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 300)) AS timing,
             avg(pos_x),
             avg(pos_y)
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 17:00:00'
      GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 300
      ORDER BY timing) as eL
GROUP BY timing
ORDER BY timing;

-- IMPORTANTE DA RIANALIZZARE
SELECT mac_addr,
       FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), 300)) AS timing,
       COUNT(DISTINCT timing)                                                   AS freq
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) AS timing,
             avg(pos_x)                                                                    AS pos_x,
             avg(pos_y)                                                                    AS pos_y
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:05:00' AND '2019-09-25 16:10:00'
      GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 60
      ORDER BY timing) as eL
GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), 300))
ORDER BY timing;

-- QUERY_3
SELECT mac_addr, timing, pos_x, pos_y
FROM (SELECT hash_fcs,
             mac_addr,
             avg(pos_x)                       as pos_x,
             avg(pos_y)                       as pos_y,
             timestamp,
             ssid,
             hidden,
             MAX(FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)))
                 OVER (PARTITION BY mac_addr) AS timing
      FROM eurecomLab
      WHERE timestamp > '2019-09-25 16:35:00'
        AND timestamp < '2019-09-25 16:40:00'
      GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 60
      ORDER BY timing) AS s2
WHERE FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) = timing
ORDER BY mac_addr, timestamp DESC;

SELECT DISTINCT(mac_addr), timestamp, pos_x, pos_y
FROM eurecomLab s1
WHERE timestamp = (SELECT MAX(timestamp) FROM eurecomLab s2 WHERE s1.mac_addr = s2.mac_addr LIMIT 1)
ORDER BY mac_addr, timestamp DESC;

-- Conteggio MAC rilevati più di 5 minuti all'interno di bucket di 5 minuti
SELECT timing, COALESCE(COUNT(*)  ,0)
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), 300)) AS timing,
             COALESCE(COUNT(DISTINCT timing)  ,0)                                                 AS freq
      FROM (SELECT mac_addr,
                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) AS timing,
                   COALESCE(COUNT(DISTINCT timestamp)  ,0)
            FROM eurecomLab
            WHERE timestamp BETWEEN '2019-09-25 16:05:00' AND '2019-09-25 17:20:00'
            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 60
            ORDER BY timing) as eL
      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), 300))
     ) AS mac_count
-- Meglio mettere 3 in questo punto per una stima migliore
WHERE mac_count.freq >= 3
GROUP BY timing
ORDER BY timing;

-- Conteggio frequenza MAC
SELECT mac_addr,
       COUNT(DISTINCT timing) AS freq,
       hidden
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 60)) AS timing,
             hidden
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 17:00:00'
      GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV 60
      ORDER BY timing) as eL
GROUP BY mac_addr;

-- FREQUENZE MAC RANGE
-- Dati inizio e fine raggruppa in piccoli gruppi di 5 minuti e calcola quanto è frequente un MAC, bucket di 5 minuti
SELECT mac_addr, COUNT(*) AS frequency, hidden
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 300)) AS timing,
             hidden
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 17:00:00'
      GROUP BY mac_addr, timing
      ORDER BY timing) as mat
GROUP BY mac_addr
ORDER BY frequency DESC;

-- TODO: sliding window
-- Conta quanti Mac ci sono nel range di tempo selezionato
SELECT timing, COUNT(DISTINCT mac_addr)
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 300)) AS timing
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 17:00:00'
      GROUP BY mac_addr, timing
      ORDER BY timing) as mat
GROUP BY timing;

-- Conta quanti Mac nascosti ci sono nel range di tempo selezionato
SELECT COUNT(DISTINCT mac_addr)
FROM eurecomLab
WHERE timestamp BETWEEN '2019-09-25 16:05:00' AND '2019-09-25 16:10:00'
  AND hidden = 1;

-- Conta quanti Mac nascosti ci sono nel range di tempo selezionato
SELECT mac_addr,
       timestamp,
       last_value(pos_x) OVER (PARTITION BY mac_addr ORDER BY timestamp ) AS X
FROM eurecomLab
WHERE timestamp > '2019-09-25 16:20:00';

SELECT DISTINCT(mac_addr), timestamp, pos_x, pos_y
FROM eurecomLab s1
WHERE timestamp = (SELECT MAX(timestamp) FROM eurecomLab s2 WHERE s1.mac_addr = s2.mac_addr LIMIT 1)
ORDER BY mac_addr, timestamp DESC;

-- Seleziona MAC nell'intervallo di tempo selezionato
SELECT mac_addr, COUNT(*) AS frequency
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 300)) AS timing
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 17:00:00'
      GROUP BY mac_addr, timing
      ORDER BY timing) as mat
GROUP BY mac_addr
ORDER BY frequency DESC;



-- FREQUENZE MAC RANGE
-- Dati inizio e fine raggruppa in piccoli gruppi di 5 minuti e calcola quanto è frequente un MAC, bucket di 5 minuti, ma MAC non hidden
SELECT mac_addr, COUNT(*) AS frequency
FROM (SELECT mac_addr,
             FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 300)) AS timing
      FROM eurecomLab
      WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 17:00:00'
        AND hidden = 0
      GROUP BY mac_addr, timing
      ORDER BY timing) as mat
GROUP BY mac_addr
ORDER BY frequency DESC;

-- MAC che compaiono almeno 2 volte nell'intervallo di tempo
SELECT mac_addr, COUNT(*)
FROM eurecomLab
WHERE timestamp BETWEEN '2019-09-25 16:00:00' AND '2019-09-25 16:10:00'
GROUP BY mac_addr
HAVING COUNT(*) > 2;

-- Seleziona i MAC dei dispositivi che sono rimasti fermi nell'intervallo di tempo
SELECT DISTINCT(mac_addr), pos_x
FROM eurecomLab
WHERE timestamp BETWEEN '2019-09-25 16:05:00' AND '2019-09-25 16:10:00'
GROUP BY pos_x, pos_y;


-- Seleziona MAC che si trovano in una certa posizione
SELECT DISTINCT(mac_addr)
FROM eurecomLab
WHERE timestamp BETWEEN '2019-09-25 16:05:00' AND '2019-09-25 16:10:00'
  AND pos_x > 2
  AND pos_y >= 0
  AND pos_x < 5
  AND pos_y < 5


