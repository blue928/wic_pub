<?php

/**
 * @file Code to define the WIC publication node type and its fields.
 */

/**
 * Create the WIC publication node type.
 */
function _wic_pub_create_node_type() {
  $type = array(
    'type' => 'wic_pub',
    'name' => t('WIC publication'),
    'base' => 'node_content',
    'description' => t('<em>WIC publications</em> are used to generate the publications order form.'),
    'has_title' => '1',
    'title_label' => t('Title'),
    'custom' => 1,
    'modified' => 1,
    'locked' => 0,
    'help' => '',
  );

  $type = node_type_set_defaults($type);
  node_type_save($type);

  // Enable node publishing for publications.
  variable_set('node_options_wic_pub', array('status'));
  // Disable comments for the publication node type.
  variable_set("comment_wic_pub", 1);

  // Define default fields and field instances.

  // Order number
  $field = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'wic_pub_order_number',
    'foreign keys' => array(
      'format' => array(
        'columns' => array(
          'format' => 'format',
        ),
        'table' => 'filter_format',
      ),
    ),
    'indexes' => array(
      'format' => array(
        0 => 'format',
      ),
    ),
    'module' => 'text',
    'settings' => array(
      'max_length' => '10',
    ),
    'translatable' => '0',
    'type' => 'text',
  );
  field_create_field($field);

  $instance = array(
    'bundle' => 'wic_pub',
    'default_value' => NULL,
    'deleted' => '0',
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'wic_pub_order_number',
    'label' => t('Order number'),
    'required' => 0,
    'settings' => array(
      'text_processing' => '0',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => '10',
      ),
      'type' => 'text_textfield',
      'weight' => '-4',
    ),
  );
  field_create_instance($instance);

  // Description
  $field = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'wic_pub_description',
    'foreign keys' => array(
      'format' => array(
        'columns' => array(
          'format' => 'format',
        ),
        'table' => 'filter_format',
      ),
    ),
    'indexes' => array(
      'format' => array(
        0 => 'format',
      ),
    ),
    'module' => 'text',
    'settings' => array(
      'max_length' => '60',
    ),
    'translatable' => '0',
    'type' => 'text',
  );
  field_create_field($field);

  $instance = array(
    'bundle' => 'wic_pub',
    'default_value' => NULL,
    'deleted' => '0',
    'description' => t('The publication description displayed on the order form'),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'text',
        'settings' => array(),
        'type' => 'text_default',
        'weight' => 1,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'wic_pub_description',
    'label' => t('Description'),
    'required' => 1,
    'settings' => array(
      'text_processing' => '0',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'size' => '60',
      ),
      'type' => 'text_textfield',
      'weight' => '-3',
    ),
  );
  field_create_instance($instance);

  // In stock
  $field = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'wic_pub_in_stock',
    'foreign keys' => array(),
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        0 => '',
        1 => '',
      ),
      'allowed_values_function' => '',
    ),
    'translatable' => '0',
    'type' => 'list_boolean',
  );
  field_create_field($field);

  $instance = array(
    'bundle' => 'wic_pub',
    'default_value' => array(
      0 => array(
        'value' => 1,
      ),
    ),
    'deleted' => '0',
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'wic_pub_in_stock',
    'label' => t('Check if the publication is in stock.'),
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(
        'display_label' => 1,
      ),
      'type' => 'options_onoff',
      'weight' => '-2',
    ),
  );
  field_create_instance($instance);

  // Maximum order quantity
  $field = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'wic_pub_max_quantity',
    'foreign keys' => array(),
    'indexes' => array(),
    'module' => 'number',
    'settings' => array(),
    'translatable' => '0',
    'type' => 'number_integer',
  );
  field_create_field($field);

  $instance = array(
    'bundle' => 'wic_pub',
    'default_value' => NULL,
    'deleted' => '0',
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'wic_pub_max_quantity',
    'label' => t('Maximum order quantity'),
    'required' => 0,
    'settings' => array(
      'max' => '999',
      'min' => '0',
      'prefix' => '',
      'suffix' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'number',
      'settings' => array(),
      'type' => 'number',
      'weight' => '-1',
    ),
  );
  field_create_instance($instance);

  // Order form category
  $field = array(
    'active' => '1',
    'cardinality' => '1',
    'deleted' => '0',
    'entity_types' => array(),
    'field_name' => 'wic_pub_term_ref_of',
    'foreign keys' => array(
      'tid' => array(
        'columns' => array(
          'tid' => 'tid',
        ),
        'table' => 'taxonomy_term_data',
      ),
    ),
    'indexes' => array(
      'tid' => array(
        0 => 'tid',
      ),
    ),
    'module' => 'taxonomy',
    'settings' => array(
      'allowed_values' => array(
        0 => array(
          'vocabulary' => 'wic_pub_of',
          'parent' => '0',
        ),
      ),
    ),
    'translatable' => '0',
    'type' => 'taxonomy_term_reference',
  );
  field_create_field($field);

  $instance = array(
    'bundle' => 'wic_pub',
    'default_value' => NULL,
    'deleted' => '0',
    'description' => t('The tagged term determines the publication\'s placement in the order form.'),
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'taxonomy',
        'settings' => array(),
        'type' => 'taxonomy_term_reference_link',
        'weight' => 4,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'wic_pub_term_ref_of',
    'label' => t('Order form category'),
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => 0,
    ),
  );
  field_create_instance($instance);
}

/**
 * Uninstall the WIC publication node type.
 */
function _wic_pub_delete_node_type() {
  $fields = array(
    'wic_pub_order_number',
    'wic_pub_description',
    'wic_pub_in_stock',
    'wic_pub_quantity_max',
    'wic_pub_term_ref_of',
  );
  foreach ($fields as $field) {
    field_delete_field($field);
    watchdog('wic_pub', 'Deleted the :field_name field from all content type instances.', array(':field_name' => $field));
  }
}
