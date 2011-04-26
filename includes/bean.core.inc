<?php

/**
 * The Bean entity class
 */
class Bean extends Entity {
  public $label;
  public $title;
  public $type;
  public $data;
  protected $plugin;

  /**
   * Get Plugin info
   */
  public function getInfo($key = NULL) {
    return $this->plugin->getInfo($key);
  }

  public function defaultLabel() {
    return $this->label;
  }

  public function __construct($values = array()) {
    parent::__construct($values, 'bean');

    // load the plugin info
    $this->plugin = bean_load_plugin_class($this->type);
    $this->setFields();
  }

  /**
   * Set the fields from the defaults and plugin
   */
  protected function setFields() {
    $values = unserialize($this->data);
    foreach ($this->plugin->values() as $field => $default) {
      $this->$field = empty($values[$field]) ? $default : $values[$field];
      dsm($this->$field);
    }
  }

  /**
   * Override this in order to implement a custom default URI and specify
   * 'entity_class_uri' as 'uri callback' hook_entity_info().
   */
  protected function defaultUri() {
    return array('path' => 'block/' . $this->identifier());
  }

  /**
   * Get the plugin form
   */
  public function getForm() {
    return $this->plugin->form($this);
  }

  /**
   * Validate the plugin form
   */
  public function validate($values) {
    $this->plugin->validate($values);
  }

  /**
   * Edit URL
   */
  public function editURL() {
    return 'block/' . $this->identifier() . '/edit';
  }

  /**
   * Set the values from a plugin
   */
  public function setValues($values) {
    $this->data = array();
    foreach ($this->plugin->values() as $field => $value) {
      $this->data[$field] = $values[$field];
    }
  }
}

/**
 * The Bean type entity class
 */
class BeanType extends Entity {
  public $weight = 0;
  public $type;
  public $label;

  public function __construct($values = array()) {
    parent::__construct($values, 'bean_type');
  }

  /**
   * Returns whether the bean type is locked, thus may not be deleted or renamed.
   *
   * Bean types provided in code are automatically treated as locked, as well
   * as any fixed bean type.
   */
  public function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }

  /**
   * URL friendly key
   */
  public function urlKey() {
    return str_replace('_', '-', $this->identifier());
  }

  /**
   * Add URL
   */
  public function addURL() {
    return 'block/add/' . $this->urlKey();
  }
}