library ieee;
use ieee.std_logic_1164.all;

entity test is
  generic (
    ASD : integer := 1
  );
  port (
    std : in std_logic;
    stdl : out std_logic_vector(0 to ASD)
  );
end entity;