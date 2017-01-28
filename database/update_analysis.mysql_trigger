CREATE TRIGGER `update_analysis` AFTER UPDATE ON `hosts_status`
 FOR EACH ROW BEGIN
INSERT INTO hosts_analysis (host_name, cpu_max) 
 SELECT hosts_details.canonical_name, NEW.cpu_usage
  FROM hosts_details
   WHERE NEW.id = hosts_details.id
 ON DUPLICATE KEY
  UPDATE cpu_max=
   CASE WHEN NEW.cpu_usage > cpu_max THEN NEW.cpu_usage ELSE cpu_max
   END;
END