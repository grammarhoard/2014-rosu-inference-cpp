<?php
/**
 * Heart of the framework
 * Enables using tables as objects
 * @author alpha
 */
class SQLQuery
{
    protected $_dbHandle;
    protected $_result;
    protected $_query;
    protected $_table;

    protected $_describe = array();

    protected $_orderBy;
    protected $_order;
    protected $_extraConditions;
    protected $_hO;
    protected $_hM;
    protected $_hMABTM;
    protected $_page;
    protected $_limit;

    /**
     * Connects to database
     * @param string $address
     * @param string $account
     * @param string $pwd
     * @param string $name
     */
    public function connect()
    {
        $this->_dbHandle = mysql_connect(Amvc::$config->db['db_host'], 
            Amvc::$config->db['db_user'], Amvc::$config->db['db_password']);

        if ($this->_dbHandle != 0) {
            if (mysql_select_db(Amvc::$config->db['db_name'], $this->_dbHandle)) {
                return 1;
            }
            return 0;
        }
        return 0;
    }

    /**
     * Disconnects from database
     */
    public function disconnect()
    {
        if (mysql_close ($this->_dbHandle) != 0) {
            return 1;
        }
        return 0;
    }

    /**
     * Set WHERE clause
     * @param string $field
     * @param string $value
     */
    public function where($field, $value)
    {
        $this->_extraConditions .= 
            "`" . $this->_model . "`.`" . $field . "` = '"
            . mysql_real_escape_string($value) . "' AND "
        ;
    }

   /**
     * Set LIKE clause
     * @param string $field
     * @param string $value
     */
    public function like($field, $value)
    {
        $this->_extraConditions .= 
            "`" . $this->_model . "`.`" . $field . "` LIKE '%"
            . mysql_real_escape_string($value) . "%' AND "
        ;
    }

    /**
     * Set ORDER BY clause
     * @param string $orderBy
     * @param string(ASC|DESC) $order
     */
    public function orderBy ($orderBy, $order = 'ASC')
    {
        $this->_orderBy = $orderBy;
        $this->_order   = $order;
    }

    /**
     * Set hasOne relationship
     */
    public function showHasOne()
    {
        $this->_hO = 1;
    }

    /**
     * Set hasMany relationship
     */
    public function showHasMany()
    {
        $this->_hM = 1;
    }

    /**
     * Set hasManyAndBelongsToMany relationship
     */
    public function showHMABTM()
    {
        $this->_hMABTM = 1;
    }

    /**
     * Set LIMIT clause
     * @param int $limit
     */
    public function setLimit($limit)
    {
        $this->_limit = $limit;
    }

    /**
     * Set current page
     * @param int $page
     */
    public function setPage($page)
    {
        $this->_page = $page;
    }

    /**
     * Build SQL Query
     * If there are no relationships => select * from tableName
     * Influence the statement by using:
     * - where('fieldName', 'value') => appends WHERE `fieldName`='value'
     * - like('fieldName', 'value') => appends WHERE `fieldName` LIKE '%value%'
     * - setPage('pageNumber') => enables pagination and display only results for the set page number
     * - setLimit('fieldName', 'value') => allows changing the number of results per page if pageNumber is set
     *     its default value is the one set in config.jsn
     * - orderBy('fieldName', 'Order') => appends order by `fieldName` ASC / DESC
     * - id = X => displays only a single result of the row matching the id
     */
    public function search()
    {
        $from = '`' . $this->_table . '` as `' . $this->_model . '` ';
        $conditions = "1=1 AND ";
        $conditionsChild = '';
        $fromChild = '';

        // showHasOne() has been called
        if ($this->_hO == 1 && isset($this->hasOne)) {

            // foreach hasOne relationship a LEFT JOIN is done
            foreach ($this->hasOne as $alias => $model) {
                $table = $this->_table;
                $singularAlias = strtolower($alias);
                $from .= 'LEFT JOIN `' . $table . '` as `' . $alias . '` ';
                $from .= 'ON `' . $this->_model . '`.`' . $singularAlias . '_id` = `' . $alias . '`.`id` ';
            }
        }// end if showHasOne()

        if ($this->id) {
            $conditions .= "`" . $this->_model . "`.`id` = '"
                . mysql_real_escape_string ($this->id) . "' AND ";
        }

        if ($this->_extraConditions) {
            $conditions .= $this->_extraConditions;
        }

        $conditions = substr($conditions, 0, -4);

        if (isset($this->_orderBy)) {
            $conditions .= ' ORDER BY `' . $this->_model . '`.`' . $this->_orderBy . '` ' . $this->_order;
        }

        if (isset($this->_page)) {
            $offset = ($this->_page - 1) * $this->_limit;
            $conditions .= ' LIMIT ' . $this->_limit . ' OFFSET ' . $offset;
        }

        $this->_query = 'SELECT * FROM ' . $from . ' WHERE ' . $conditions;
        # echo '<!--' . $this->_query . '-->';
        $this->_result = mysql_query($this->_query, $this->_dbHandle);

        $result      = array();
        $table       = array();
        $field       = array();
        $tempResults = array();
        $numOfFields = mysql_num_fields($this->_result);
        for ($i = 0; $i < $numOfFields; ++$i) {
            array_push($table, mysql_field_table($this->_result, $i));
            array_push($field, mysql_field_name($this->_result, $i));
        }


        // 0 rows returned
        if (mysql_num_rows($this->_result) <= 0) {
            mysql_free_result($this->_result);
            $this->clear();
            return $result;
        }

        // at least 1 row returned
        $row = mysql_fetch_row($this->_result);
        while ($row) {
            for ($i = 0; $i < $numOfFields; ++$i) {
                $tempResults[$table[$i]][$field[$i]] = $row[$i];
            }

            // showHasMany() has been called
            // 1: Many Relationship
            // children table will have a field called `parent_id`
            if ($this->_hM == 1 && isset($this->hasMany)) {

                // for each result returned by the above query and for each hasMany relationship
                // it will find all those records in the second table which match the current result's id
                // then it will push all those results in the same array
                // i.e. if teachers hasMany students => $this->Teacher->showHasMany() 
                //     will search for teacher_id in the students table 
                foreach ($this->hasMany as $aliasChild => $modelChild) {
                    $queryChild      = '';
                    $conditionsChild = '';
                    $fromChild       = '';

                    // $tableChild = strtolower ($inflect->pluralize ($modelChild));
                    // $pluralAliasChild = strtolower ($inflect->pluralize ($aliasChild));

                    //TODO: in PHP 5.3 you can use $tableChild = $modelChild::$_tableName;
                    // eval('$tableChild = '.$modelChild.'::$_tableName;');
                    $reflection = new ReflectionClass($modelChild);
                    $tableChild = $reflection->getStaticPropertyValue('_tableName');

                    $pluralAliasChild = $aliasChild;
                    $singularAliasChild = strtolower($aliasChild);

                    $fromChild .= '`' . $tableChild . '` as `' . $aliasChild . '`';

                    $conditionsChild .= "`" . $aliasChild . "`.`" . strtolower ($this->_model)
                        . "_id` = '" . $tempResults[$this->_model]['id'] . "'";

                    $queryChild = 'SELECT * FROM ' . $fromChild . ' WHERE ' . $conditionsChild;
                    # echo '<!--' . $queryChild . '-->';
                    $resultChild = mysql_query($queryChild, $this->_dbHandle);

                    $tableChild       = array();
                    $fieldChild       = array();
                    $tempResultsChild = array();
                    $resultsChild     = array();

                    if (mysql_num_rows($resultChild) > 0) {
                        $numOfFieldsChild = mysql_num_fields ($resultChild);
                        for ($j = 0; $j < $numOfFieldsChild; ++$j) {
                            array_push($tableChild, mysql_field_table($resultChild, $j));
                            array_push($fieldChild, mysql_field_name($resultChild, $j));
                        }

                        $rowChild = mysql_fetch_row($resultChild);
                        while ($rowChild) {
                            for ($j = 0; $j < $numOfFieldsChild; ++$j) {
                                $tempResultsChild[$tableChild[$j]][$fieldChild[$j]] = $rowChild[$j];
                            }
                            array_push($resultsChild, $tempResultsChild);
                            $rowChild = mysql_fetch_row($resultChild);
                        }
                    }

                    $tempResults[$aliasChild] = $resultsChild;
                    mysql_free_result($resultChild);
                }
            }// end showHasMany()


            // showHMBTM() has been called
            // Many: Many Relationship
            // Naming convention for this table is alphabetical.
            // i.e. if the tables are cars and animals, then the table should be named animals_cars
            if ($this->_hMABTM == 1 && isset($this->hasManyAndBelongsToMany)) {

                // foreach result returned by the first query and for each hasManyAndBelongsToMany relationship
                // it will find all those records in the second table which match the current result's id
                // i.e. if teachers hasManyAndBelongsToMany students => this->Teacher->showHMABTM()
                //     will search for teacher_id in students_teachers table
                foreach ($this->hasManyAndBelongsToMany as $aliasChild => $tableChild) {
                    $queryChild      = '';
                    $conditionsChild = '';
                    $fromChild       = '';

                    // $tableChild = strtolower ($inflect->pluralize ($tableChild));

                    //TODO: in PHP 5.3 you can use $tableChild = $modelChild::$_tableName;
                    // eval('$tableChild = '.$modelChild.'::$_tableName;');
                    $reflection = new ReflectionClass($tableChild);
                    $tableChild = $reflection->getStaticPropertyValue('_tableName');

                    $sortTables = array($this->_table, $aliasChild);
                    sort($sortTables);
                    $joinTable = implode('_', $sortTables);

                    $fromChild .= '`' . $tableChild . '` as `' . $aliasChild . '`,';
                    $fromChild .= '`' . $joinTable . '`,';

                    $conditionsChild .= '`' . $joinTable . '`.`' . $aliasChild . '_id` = `' . $aliasChild . '`.`id` AND ';
                    $conditionsChild .= "`" . $joinTable . "`.`" . strtolower($this->_model) . "_id` = '" . $tempResults[$this->_model]['id'] . "'";
                    $fromChild = substr($fromChild, 0, -1);

                    $queryChild = 'SELECT * FROM ' . $fromChild . ' WHERE ' . $conditionsChild;
                    # echo '<!--' . $queryChild . '-->';
                    $resultChild = mysql_query ($queryChild, $this->_dbHandle);

                    $tableChild       = array();
                    $fieldChild       = array();
                    $tempResultsChild = array();
                    $resultsChild     = array();

                    if (mysql_num_rows($resultChild) > 0) {
                        $numOfFieldsChild = mysql_num_fields($resultChild);
                        for ($j = 0; $j < $numOfFieldsChild; ++$j) {
                            array_push($tableChild, mysql_field_table($resultChild, $j));
                            array_push($fieldChild, mysql_field_name($resultChild, $j));
                        }

                        $rowChild = mysql_fetch_row($resultChild);
                        while ($rowChild) {
                            for ($j = 0; $j < $numOfFieldsChild; ++$j) {
                                $tempResultsChild[$tableChild[$j]][$fieldChild[$j]] = $rowChild[$j];
                            }
                            array_push($resultsChild, $tempResultsChild);
                            $rowChild = mysql_fetch_row($resultChild);
                        }
                    }

                    $tempResults[$aliasChild] = $resultsChild;
                    mysql_free_result($resultChild);
                }
            }// end showHMBTM()

            array_push($result, $tempResults);
            $row = mysql_fetch_row($this->_result);

        } // end while($row)

        $numRows = mysql_num_rows($this->_result);
        $id = $this->id;
        mysql_free_result($this->_result);
        $this->clear();

        // if id is set => it will return a single result and not an array
        if ($numRows == 1 && $id != null) {
            return $result[0];
        }
        return $result;
    }

    /**
     * Build custom SQL Query
     * @param string $query
     */
    public function custom($query)
    {
        $result      = array();
        $table       = array();
        $field       = array();
        $tempResults = array();

        // Check if $query includes a SELECT clause
        if(substr_count(strtoupper($query), 'SELECT') <= 0) {
            $this->clear();
            return $result;
        }

        $this->_result = mysql_query($query, $this->_dbHandle);

        // 0 rows returned
        if (mysql_num_rows($this->_result) <= 0) {
            mysql_free_result($this->_result);
            $this->clear();
            return $result;
        }

        // at least 1 row returned
        $numOfFields = mysql_num_fields($this->_result);
        for ($i = 0; $i < $numOfFields; ++$i) {
            array_push($table, mysql_field_table($this->_result, $i));
            array_push($field, mysql_field_name($this->_result, $i));
        }

        $row = mysql_fetch_row($this->_result);
        while ($row) {
            for ($i = 0; $i < $numOfFields; ++$i) {
                $tempResults[$table[$i]][$field[$i]] = $row[$i];
            }
            array_push($result, $tempResults);
            $row = mysql_fetch_row($this->_result);
        }

        mysql_free_result($this->_result);
        $this->clear();
        return $result;
    }

    /**
     * Describe table
     * - store data to cache file
     */
    protected function _describe()
    {
        $this->_describe = Amvc::$cache->get('describe' . $this->_table);

        if (!$this->_describe) {
            $this->_describe = array ();
            $query = 'DESCRIBE ' . $this->_table;
            $this->_result = mysql_query($query, $this->_dbHandle);
            $row = mysql_fetch_row($this->_result);
            while ($row) {
                 array_push($this->_describe,$row[0]);
                 $row = mysql_fetch_row($this->_result);
            }

            mysql_free_result($this->_result);
            Amvc::$cache->set('describe' . $this->_table, $this->_describe);
        }

        foreach ($this->_describe as $field) {
            $this->$field = null;
        }
    }

    /**
     * Delete a record from the table
     */
    public function delete()
    {
        if (!$this->id) {
            return -1;
        }

        $query = "DELETE FROM " . $this->_table
            . " WHERE `id`='" . mysql_real_escape_string($this->id) . "'";
        $this->_result = mysql_query($query, $this->_dbHandle);

        $this->clear();
        if ($this->_result == 0) {
            //TODO Error Generation
            return -1;
        }

        //TODO Error Generation
        return -1;
    }

    /**
     * Saves an Object i.e. Updates / Inserts Query
     * Must be used from the controller
     */
    public function save()
    {
        $query = '';
        if (isset($this->id)) {
            $updates = '';
            foreach ($this->_describe as $field) {
                if ($this->$field) {
                    $updates .= "`" . $field."` = '" . mysql_real_escape_string($this->$field) . "',";
                }
            }
            $updates = substr($updates, 0, -1);

            $query = "UPDATE " . $this->_table . " SET " . $updates
                . " WHERE `id`='" . mysql_real_escape_string($this->id)."'";
        } else {
            $fields = '';
            $values = '';
            foreach ($this->_describe as $field) {
                if ($this->$field) {
                    $fields .= '`' . $field . '`,';
                    $values .= "'".mysql_real_escape_string($this->$field)."',";
                }
            }
            $values = substr ($values, 0, -1);
            $fields = substr ($fields, 0, -1);

            $query = 'INSERT INTO ' . $this->_table . ' (' . $fields . ') VALUES (' . $values . ')';
        }
        $this->_result = mysql_query($query, $this->_dbHandle);
        $this->clear();
        if ($this->_result == 0) {
            //TODO Error Generation
            return -1;
        }
    }

    /**
     * Clear all variables
     */
    public function clear()
    {
        foreach($this->_describe as $field) {
            $this->$field = null;
        }

        $this->_orderby         = null;
        $this->_extraConditions = null;
        $this->_hO              = null;
        $this->_hM              = null;
        $this->_hMABTM          = null;
        $this->_page            = null;
        $this->_order           = null;
    }

    /**
     * Pagination Count
     */
    public function totalPages()
    {
        if ($this->_query && $this->_limit) {
            $pattern = '/SELECT (.*?) FROM (.*)LIMIT(.*)/i';
            $replacement = 'SELECT COUNT(*) FROM $2';
            $countQuery = preg_replace($pattern, $replacement, $this->_query);
            $this->_result = mysql_query($countQuery, $this->_dbHandle);
            $count = mysql_fetch_row($this->_result);
            $totalPages = ceil($count[0] / $this->_limit);
            return $totalPages;
        }

        //TODO Error Generation
        return -1;
    }

    /**
     * Get mysql_error string
     */
    public function getError()
    {
        //TODO log errors
        return mysql_error($this->_dbHandle);
    }
}