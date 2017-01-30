CREATE TRIGGER `update_analysis` AFTER UPDATE ON `hosts_status`
 FOR EACH ROW BEGIN
INSERT INTO hosts_analysis 
 (
  host_name,
  cpu_max,
  cpu_max_timestamp,
  cpu_min,
  cpu_min_timestamp,
  memory_max,
  memory_max_timestamp,
  memory_min,
  memory_min_timestamp,
  disk_max,
  disk_max_timestamp,
  disk_min,
  disk_min_timestamp
 ) 
 SELECT
  hosts_details.canonical_name,
  NEW.cpu_usage,
  NEW.cpu_timestamp,
  NEW.cpu_usage,
  NEW.cpu_timestamp,
  NEW.memory_usage,
  NEW.memory_timestamp,
  NEW.memory_usage,
  NEW.memory_timestamp,
  NEW.disk_usage,
  NEW.disk_timestamp,
  NEW.disk_usage,
  NEW.disk_timestamp
  FROM hosts_details
   WHERE NEW.id = hosts_details.id
 ON DUPLICATE KEY
  UPDATE
   cpu_max=
   CASE WHEN NEW.cpu_usage > cpu_max THEN NEW.cpu_usage ELSE cpu_max
   END,
   cpu_max_timestamp=
   CASE WHEN NEW.cpu_usage > cpu_max THEN NEW.cpu_timestamp ELSE cpu_max_timestamp
   END,
   cpu_min=
   CASE WHEN NEW.cpu_usage < cpu_min THEN NEW.cpu_usage ELSE cpu_min
   END,
   cpu_min_timestamp=
   CASE WHEN NEW.cpu_usage < cpu_min THEN NEW.cpu_timestamp ELSE cpu_min_timestamp
   END,
   memory_max=
   CASE WHEN NEW.memory_usage > memory_max THEN NEW.memory_usage ELSE memory_max
   END,
   memory_max_timestamp=
   CASE WHEN NEW.memory_usage > memory_max THEN NEW.memory_timestamp ELSE memory_max_timestamp
   END,
   memory_min=
   CASE WHEN NEW.memory_usage < memory_min THEN NEW.memory_usage ELSE memory_min
   END,
   memory_min_timestamp=
   CASE WHEN NEW.memory_usage < memory_min THEN NEW.memory_timestamp ELSE memory_min_timestamp
   END,
   disk_max=
   CASE WHEN NEW.disk_usage > disk_max THEN NEW.disk_usage ELSE disk_max
   END,
   disk_max_timestamp=
   CASE WHEN NEW.disk_usage > disk_max THEN NEW.disk_timestamp ELSE disk_max_timestamp
   END,
   disk_min=
   CASE WHEN NEW.disk_usage < disk_min THEN NEW.disk_usage ELSE disk_min
   END,
   disk_min_timestamp=
   CASE WHEN NEW.disk_usage < disk_min THEN NEW.disk_timestamp ELSE disk_min_timestamp
   END;
END
