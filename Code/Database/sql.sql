 CREATE TABLE weightdata (

  id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,

  petWeight VARCHAR(30) not null,

  feedWeight VARCHAR(30) not null,

  reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP

);
